#include "WebService.h"

WebService *WebService::_instance = NULL;

const char *WebService::_ssid = "openLedRace";
const char *WebService::_password = "1234led56";

const char WebService::_index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html lang='fr'>
    <head>
        <title>OpenLedRace Configuration</title>
        <meta http-equiv='refresh' content='60' name='viewport' content='width=device-width, initial-scale=1' charset='UTF-8'/>
        <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'> 
    </head>

    <body>
      <div class='w3-card w3-blue w3-padding-small w3-jumbo w3-center'>
        <p>Race Status: %RACE_STATUS% </p>
      </div>

      <div class='w3-bar'>
        <a href='/Start' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>Start</a>
        <a href='/Stop' class='w3-bar-item w3-button w3-border w3-jumbo' style='width:50%; height:50%;'>Stop</a>
      </div>
      <br></br>
      <div class='w3-bar'>
      <form action='/get'>
        Nombre de LEDs (%MaxLED%)   : <input type='text' name='MaxLED'>
        <input type='submit' value='Submit'>
      </form><br>
      <form action='/get'>
        Nombre de tours (%MaxLoops%): <input type='text' name='MaxLoops'>
        <input type='submit' value='Submit'>
      </form><br>
      <form action='/get'>
        Gravité (%kg%)              : <input type='text' name='kg'>
        <input type='submit' value='Submit'>
      </form>
      </div>

      <div class='w3-center w3-padding-16'>
        <p>Liffab</p>
        <i>Enjoy !</i>
      </div>

    </body>
    </html>
)rawliteral";

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
    Serial.begin(115200);

    Serial.println("Starting web server...");
    WiFi.softAP(WebService::_ssid, WebService::_password);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("Server IP address: ");
    Serial.println(IP);

    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send_P(200, "text/html", _index_html, processor); });

    _server.on("/Start", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceState = RACE_STARTED;
                   request->send_P(200, "text/html", _index_html, processor);
               });

    _server.on("/Stop", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   RaceState = RACE_STOPPED;
                   request->send_P(200, "text/html", _index_html, processor);
               });

    _server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
               {
                   String input_value;

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
                       Serial.printf("MaxLoops modified to %d\n", MaxLoops);
                   }
                   else
                   {
                       input_value = "No message sent";
                   }
                   Serial.println(input_value);
                   request->send_P(200, "text/html", _index_html, processor);
               });

    _server.begin();
}

void WebService::notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

String WebService::processor(const String &var)
{
    if (var == "RACE_STATUS")
    {
        switch (RaceState)
        {
        case RACE_STOPPED:
            return "Stopped";
        case RACE_STARTED:
            return "Running";
        case RACE_CONFIGURING:
            return "Configuring";
        }
    }
    else if (var == "MaxLoops")
    {
        return String(MaxLoops);
    }
    else if(var == "MaxLED")
    {
      return String(MaxLED);
    }
    return String();
}