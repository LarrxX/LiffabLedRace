#include "WebService.h"

#include "Car.h"
#include "OilObstacle.h"
#include "RampObstacle.h"
#include "ColorUtils.h"

#include "CSS.h"

using namespace RaceConfig;
using namespace ColorUtils;

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

    _server.on("/general", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   WebService::Instance().modifyGeneral(request);
                   request->send_P(200, "text/html", _index_html.c_str(), processor);
               });
    
    _server.begin();
}

void WebService::modifyGeneral( AsyncWebServerRequest *request)
{
    MaxLED = request->getParam("MaxLED")->value().toInt();
    MaxLoops = request->getParam("MaxLoops")->value().toInt();
    track.updateLength(MaxLED);
}

void WebService::modifyPlayer(AsyncWebServerRequest *request)
{
    RaceStarted = false;

    int index = request->getParam("Index")->value().toInt();
    String newName = request->getParam("Name")->value();
    uint32_t newColor = FromHTMLColor(request->getParam("Color")->value().c_str());

    if (!newName.isEmpty())
    {
        Players[index].setName(const_cast<char *>(newName.c_str()));
    }
    Players[index].mutableCar().setColor(newColor);
    buildIndexHTML();
}

void WebService::modifyObstacle(AsyncWebServerRequest *request)
{
    int index = request->getParam("Index")->value().toInt();
    IObstacle::ObstacleType type = (IObstacle::ObstacleType)(request->getParam("Type")->value().toInt());
    int start = request->getParam("Start")->value().toInt();
    int end = request->getParam("End")->value().toInt();
    uint32_t color = FromHTMLColor(request->getParam("Color")->value().c_str());

    IObstacle* obstacle = Obstacles[index];
    obstacle->setStart(start);
    obstacle->setEnd(end);
    obstacle->setColor(color);

    switch (type)
    {
    case IObstacle::OBSTACLE_OIL:
    {
        OilObstacle *oil = static_cast<OilObstacle *>(obstacle);
        oil->setPressDelay(request->getParam("Delay")->value().toInt());
    }
    break;

    case IObstacle::OBSTACLE_RAMP:
    {
        RampObstacle *ramp = static_cast<RampObstacle *>(obstacle);
        ramp->setHeight(request->getParam("Height")->value().toInt());
        ramp->setStyle((RampObstacle::RampStyle)request->getParam("Style")->value().toInt());
    }
    break;
    }
    buildIndexHTML();
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
    _players_html = "<div class='w3-bar'><br><hr style='height:3px;color:black;background-color:black'><br><h2>Players</h2>";

    //<form action='/player'>
    //<input type='color' name='Color' value=#COLOR>1 - Name1 <input type='text' name='Name' size = MAX_NAME_LENGTH>
    //<input type='hidden' name='Index' value='i'>
    //<input type='submit'><br>
    //</form><br>
    for (word i = 0; i < Players.Count(); ++i)
    {
        char color[8];
        ToHTMLColor(Players[i].car().getColor(), color);

        _players_html += "<form action='/player'><input type='color' name='Color' value='"
        + String(color)
        + "'> "
        + String(i + 1)
        + " <input type='text' name='Name' value='"
        + Players[i].getName()
        + "' size="
        + String(MAX_NAME_LENGTH)
        + "><input type='hidden' name='Index' value='"
        + String(i)
        + "'><input type='submit'></form><br>";
    }

    _players_html += "</div>";
}

void WebService::buildObstaclesHTML()
{
    _obstacles_html = "<div class='w3-bar'><br><hr style='height:3px;color:black;background-color:black'><br><h2>Obstacles</h2>";

    String typeName;
    String specifics;
    //<form action='/obstaclei'>
    //<span style="background-color: rgba(255, 0, 0, 1);">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>1 - Type: start(%start%) <input type='text' name='Start' size=5>
    // end(%end%) <input type='text' name='End' size=5>
    //<input type='hidden' name='Index' value='i'>
    //<input type='hidden' name='Type' value=%type%>
    //specifics
    //<input type='submit'><br>
    //</form><br>
    for (word i = 0; i < Obstacles.Count(); ++i)
    {
        char color[8];
        ToHTMLColor(Obstacles[i]->getColor(), color);

        switch (Obstacles[i]->getType())
        {
        case IObstacle::OBSTACLE_OIL:
        {
            typeName = "Oil";
            OilObstacle *oil = static_cast<OilObstacle *>(Obstacles[i]);
            //Delay (%delay%): <input type='text' name='Delay' size=4>
            specifics = " Delay <input type='text' name='Delay' value='" + String(oil->getPressDelay()) + "'size=5>";
        }
        break;

        case IObstacle::OBSTACLE_RAMP:
            typeName = "Ramp";
            RampObstacle* ramp = static_cast<RampObstacle *>(Obstacles[i]);
            //Height (%height%): <input type='text' name='Height' size=2>
            //Style <select name='Style'>
            //<option value=0>Style0</option>
            //...
            //<option value=N selected>StyleN</option>
            //</select>
            specifics = " Height <input type='text' name='Height' value='"
            + String(ramp->getHeight())
            + "' size=2>"
            + " Style <select name='Style'>";

            for (int s = 0; s < RampObstacle::RAMP_STYLE_END; ++s)
            {
                specifics += "<option value='" + String(s) + "'";
                if (ramp->getStyle() == s)
                {
                    specifics += " selected";
                }
                specifics += ">" + String(RampObstacle::getStyleName((RampObstacle::RampStyle)s)) + "</option>";
            }
            specifics += "</select>";
            break;
        }

        _obstacles_html += "<form action='/obstacle'><input type='color' name='Color' value='"
        + String(color)
        + "'> "
        + String(i + 1)
        + " - <b>"
        + typeName
        + "</b>: Start <input type='text' name='Start' value='"
        + String(Obstacles[i]->getStart())
        + "' size=5> End <input type='text' name='End' value='" 
        + String(Obstacles[i]->getEnd())
        + "' size=5><input type='hidden' name='Index' value='"
        + String(i)
        + "'><input type='hidden' name='Type' value='"
        + String(Obstacles[i]->getType())
        + "'>"
        + specifics
        + " <input type='submit'></form><br>";
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
        <style>
        )rawliteral"
    + String(css)
    + R"rawliteral(
        </style>
    </head>
    <body>
    <div class='w3-center w3-padding-16'>
        <H1>Liffab</H1>
        <H2>Open LED Race Configuration</H2>
      </div>
      <div class='w3-card w3-blue w3-padding-small w3-jumbo w3-center'>
        <p>Race Status: %RACE_STATUS% </p>
      </div>
      <div class='w3-center'>
        <a href='/Start' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>Start</a>
        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
        <a href='/Stop' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>Stop</a>
      </div>
      <div class='w3-bar'>
      <br><hr style="height:3px;color:black;background-color:black"><br>
      <h2>General</h2>
      <form action='/general'>
        Circuit LED count: <input type='text' name='MaxLED' value='%MaxLED%' size=5>
        <br>
        Number of loops: <input type='text' name='MaxLoops' value='%MaxLoops%' size=2>
        <br>
        <input type='submit'>
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