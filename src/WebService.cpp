#include "WebService.h"

#include "Car.h"
#include "OilObstacle.h"
#include "RampObstacle.h"

WebService *WebService::_instance = NULL;

const char *WebService::_ssid = "openLedRace";
const char *WebService::_password = "1234led56";

String WebService::_index_html = "";
String WebService::_players_html = "";
String WebService::_obstacles_html = "";

WebService::WebService() : _server(80)
{
}

WebService &WebService::Instance()
{
    if (_instance == NULL)
    {
        _instance = new WebService();
    }
    return *_instance;
}

void WebService::Init()
{
    buildIndexHTML();

    Serial.begin(115200);

    Serial.println("Starting web server...");
    WiFi.softAP(WebService::_ssid, WebService::_password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("Server IP address: ");
    Serial.println(IP);

    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send_P(200, "text/html", _index_html.c_str(), processor); });

    _server.on("/Start", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceStarted = true;
                   request->send_P(200, "text/html", _index_html.c_str(), processor);
               });

    _server.on("/Stop", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceStarted = false;
                   request->send_P(200, "text/html", _index_html.c_str(), processor);
               });

    _server.on("/player", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().modifyPlayer(request);
                   request->send_P(200, "text/html", _index_html.c_str(), processor);
               });

    _server.on("/obstacle", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().modifyObstacle(request);
                   request->send_P(200, "text/html", _index_html.c_str(), processor);
               });

    _server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   String input_value;
                   bool requestOK = true;
                   if (request->hasParam("MaxLED"))
                   {
                       input_value = request->getParam("MaxLED")->value();
                       MaxLED = input_value.toInt();
                       ResetTrack();
                   }
                   else if (request->hasParam("MaxLoops"))
                   {
                       input_value = request->getParam("MaxLoops")->value();
                       MaxLoops = input_value.toInt();
                   }
                   else
                   {
                       input_value = "No message sent";
                       requestOK = false;
                   }

                   if (requestOK)
                   {
                       RaceStarted = false;
                   }

                   Serial.println(input_value);
                   request->send_P(200, "text/html", _index_html.c_str(), processor);
               });

    _server.begin();
}

void WebService::modifyPlayer(AsyncWebServerRequest *request)
{
    if (request->hasParam("PlayerName") && request->hasParam("index"))
    {   
        RaceStarted = false;

        int index = request->getParam("index")->value().toInt();
        String newName = request->getParam("PlayerName")->value();
        uint32_t newColor = FromHTMLColor(request->getParam("color")->value().c_str());

        if( !newName.isEmpty() )
        {
            Players[index].setName(const_cast<char *>(newName.c_str()));
        }
        Players[index].mutableCar().setColor(newColor);
        buildIndexHTML();
    }
}

void WebService::modifyObstacle(AsyncWebServerRequest *request)
{
    int index;
    IObstacle::ObstacleType type;
    if (request->hasParam("type") && request->hasParam("index"))
    {
        index = request->getParam("index")->value().toInt();
        type = (IObstacle::ObstacleType)(request->getParam("type")->value().toInt());
        switch (type)
        {
        case IObstacle::ObstacleType::OBSTACLE_OIL:
        {
            OilObstacle* oil = static_cast<OilObstacle*>(Obstacles[index]);
            oil->setPressDelay(request->getParam("delay")->value().toInt());
        }
        break;
        
        case IObstacle::ObstacleType::OBSTACLE_RAMP:
            break;
        }
        buildIndexHTML();
    }
}

void WebService::notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

String WebService::processor(const String &var)
{
    if (var == "RACE_STATUS")
    {
        return RaceStarted ? "Started" : "Stopped";
    }
    else if (var == "MaxLoops")
    {
        return String(MaxLoops);
    }
    else if (var == "MaxLED")
    {
        return String(MaxLED);
    }
    return String();
}

void WebService::buildPlayersHTML()
{
    _players_html = "<div class='w3-bar'><h2>Players</h2>";

    //<form action='/player'>
    //<input type='color' name='color' value=#COLOR>1 - Name1 <input type='text' name='PlayerName' size = MAX_NAME_LENGTH>
    //<input type='hidden' name='index' value='i'>
    //<input type='submit' value='Submit'><br>
    //</form><br>
    for (word i = 0; i < Players.Count(); ++i)
    {
        char color[7];
        ToHTMLColor(Players[i].car().getColor(), color);

        _players_html += "<form action='/player'><input type='color' name='color' value='"
        + String(color)
        + "'> "
        + String(i + 1)
        + " - "
        + Players[i].getName()
        + " <input type='text' name='PlayerName' size="
        + String(MAX_NAME_LENGTH)
        + "><input type='hidden' name='index' value='"
        + String(i)
        + "'><input type='submit' value='Submit'></form><br>";
    }

    _players_html += "</div>";
}

void WebService::buildObstaclesHTML()
{
    _obstacles_html = "<div class='w3-bar'><h2>Obstacles</h2>";

    String typeName;
    String specifics;
    //<form action='/obstaclei'>
    //<span style="background-color: rgba(255, 0, 0, 1);">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>1 - Type: start(%start%) <input type='text' name='Start' size=5>
    // end(%end%) <input type='text' name='End' size=5>
    //<input type='hidden' name='index' value='i'>
    //<input type='hidden' name='type' value=%type%>
    //specifics
    //<input type='submit' value='Submit'><br>
    //</form><br>
    for (word i = 0; i < Obstacles.Count(); ++i)
    {
        uint8_t r, g, b;
        SplitColor(Obstacles[i]->getColor(), r, g, b);

        switch (Obstacles[i]->getType())
        {
        case IObstacle::ObstacleType::OBSTACLE_OIL:
            {
                typeName = "Oil";
                OilObstacle* oil = static_cast<OilObstacle*>(Obstacles[i]);
                //Delay (%delay%): <input type='text' name='delay' size=4>
                specifics = "Delay("
                + String(oil->getPressDelay())
                + ") <input type='text' name='delay' size=5>";
            }
            break;

        case IObstacle::ObstacleType::OBSTACLE_RAMP:
            typeName = "Ramp";
            specifics = "";
            break;
        }

        _obstacles_html += "<form action='/obstacle'><span style='background-color: rgba("
        + String(r)
        + ","
        + String(g)
        + ","
        + String(b)
        + ", 1);'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>&nbsp;"
        + String(i + 1)
        + " - "
        + typeName
        + ": Start ("
        + String(Obstacles[i]->getStart())
        + ") <input type='text' name='Start' size=5>"
        + " End ("
        + String(Obstacles[i]->getEnd())
        + ") <input type='text' name='End' size=5><input type='hidden' name='index' value='"
        + String(i)
        + "'><input type='hidden' name='type' value='"
        + String(Obstacles[i]->getType())
        + "'>"
        + specifics
        + "<input type='submit' value='Submit'></form><br>";
    }
    _obstacles_html += "</div>";
}

void WebService::buildIndexHTML()
{
    buildPlayersHTML();
    buildObstaclesHTML();

    _index_html = R"rawliteral(
    <!DOCTYPE html>
    <html lang='fr'>
    <head>
        <title>OpenLedRace Configuration</title>
        <meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1' charset='UTF-8'/>
        <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'> 
    </head>
    <body>
    <div class='w3-center w3-padding-16'>
        <H1>Liffab</H1>
        <H2>Open LED Race Configuration</H2>
      </div>
      <div class='w3-card w3-blue w3-padding-small w3-jumbo w3-center'>
        <p>Race Status: %RACE_STATUS% </p>
      </div>
      <div class='w3-bar'>
        <a href='/Start' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>Start</a>
        <a href='/Stop' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>Stop</a>
      </div>
      <br><hr><br>
      <div class='w3-bar'>
      <form action='/get'>
        Nombre de LEDs (%MaxLED%)   : <input type='text' name='MaxLED' size=5>
        <input type='submit' value='Submit'>
      </form><br>
      <form action='/get'>
        Nombre de tours (%MaxLoops%): <input type='text' name='MaxLoops' size=2>
        <input type='submit' value='Submit'>
      </form><br>
      </div>
      )rawliteral";

    _index_html += _players_html;
    _index_html += _obstacles_html;

    _index_html += R"rawliteral(
     </body>
     </html>
 )rawliteral";
}