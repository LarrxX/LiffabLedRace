#pragma once

// Webervice : include
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "RaceConfig.h"
using namespace RaceConfig;

class WebService
{
private:
  WebService();
  static WebService *_instance;

public:
  static WebService &Instance();
  void Init();
  void modifyPlayer( byte index, AsyncWebServerRequest *request);

private:
  AsyncWebServer _server;

  // Variable to store the HTTP request
  String _header;
  IPAddress _IP;

  // Webervice Wifi credentials
  static const char *_ssid;
  static const char *_password;

  static String _index_html;
  static String _players_html;
  static String _obstacles_html;

  
  void notFound(AsyncWebServerRequest *request);

  void buildPlayersHTML();
  void buildObstaclesHTML();
  void buildIndexHTML();

  static String processor(const String &var);
  
};
