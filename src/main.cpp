
/*  
 * ____                     _      ______ _____    _____
  / __ \                   | |    |  ____|  __ \  |  __ \               
 | |  | |_ __   ___ _ __   | |    | |__  | |  | | | |__) |__ _  ___ ___ 
 | |  | | '_ \ / _ \ '_ \  | |    |  __| | |  | | |  _  // _` |/ __/ _ \
 | |__| | |_) |  __/ | | | | |____| |____| |__| | | | \ \ (_| | (_|  __/
  \____/| .__/ \___|_| |_| |______|______|_____/  |_|  \_\__,_|\___\___|
        | |                                                             
        |_|          
 Open LED Race
 An minimalist cars race for LED strip  
  
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 @author LarrxX 

 heavily modified version of the original
 by gbarbarov@singulardevices.com  for Arduino day Seville 2019 
 https://www.hackster.io/gbarbarov/open-led-race-a0331a
 https://twitter.com/openledrace
 
 
 https://gitlab.com/open-led-race
 https://openledrace.net/open-software/
*/
#include "Defines.h"

#ifdef USE_SPIFFS
#include <SPIFFS.h>
#else
#include <EEPROM.h>
#endif

#include "Coroutines/PlaySoundCoroutine.h"

#include "Controller.h"
#include "Car.h"

#include "RampObstacle.h"
#include "OilObstacle.h"

#include "WebService.h"

using namespace RaceConfig;

static const word win_music[] = {
    2637, 2637, 0, 2637,
    0, 2093, 2637, 0,
    3136};

byte drawOrder[MAX_PLAYERS];
unsigned long previousRedraw = 0;
unsigned long raceStartTime = 0;
bool raceRunning = false;

Controller StartRaceButton(PIN_START);
Controller EasyModeSwitch(PIN_EASY);

PlaySoundCoroutine audio;

void ResetPlayers()
{
  for (byte i = 0; i < Players.Count(); ++i)
  {
    Players[i].Reset();
  }
}

void ResetCoroutines()
{
  audio.reset();
  trackLighting.reset();

#ifdef LED_CIRCLE
  circleLighting.reset();
#endif
}

void start_race()
{
  raceRunning = true;
  ResetCoroutines();
  ResetPlayers();

  for (word i = 0; i < MaxLED; i++)
  {
    track.setPixelColor(i, track.Color(0, 0, 0));
  };

  for (byte i = 0; i < Obstacles.Count(); ++i)
  {
    Obstacles[i]->Draw(&track);
  }
  track.show();

  #ifdef LED_CIRCLE
    circleLighting.setParameters(LedLightingCoroutine::RAINBOW, 0, 3);
    circleLighting.runBlocking();
    circle.fill(circle.Color(255,0,0),0,MAXLEDCIRCLE);
    circle.show();
  #endif

  audio.setParameters(400, 2000);
  audio.runBlocking();

  #ifdef LED_CIRCLE
    circle.fill(circle.Color(255,165,0),0,MAXLEDCIRCLE);
    circle.show();
  #endif

  audio.setParameters(600, 2000);
  audio.runBlocking();


  #ifdef LED_CIRCLE
    circle.fill(circle.Color(0,255,0),0,MAXLEDCIRCLE);
    circle.show();
  #endif

  audio.setParameters(1200, 2000);

  raceStartTime = millis();
}

void setup()
{
  raceRunning = false;

#ifdef USE_SPIFFS
  if (SPIFFS.begin(true))
  {
    Serial.println("SPIFFS successfully mounted \\o/");
  }
  else
  {
    Serial.println("Error mounting SPIFFS.");
  }

#elif defined(ESP32)
  EEPROM.begin(EEPROM_SIZE);
#endif

  if (!RaceConfig::Load())
  {
    Serial.println("No configuration file found, initializing defaults.");
    INIT_PLAYERS
    INIT_OBSTACLES
    Obstacles.Sort();
  }

  WebService::Instance().Init();

  for (byte i = 0; i < Players.Count(); ++i)
  {
    drawOrder[i] = i;
  }

  track.begin();

#ifdef LED_CIRCLE
  circle.begin();
  circle.setBrightness(125);
#endif

  start_race();
}

void winner_fx(byte w)
{
  word msize = sizeof(win_music) / sizeof(word);
  for (word note = 0; note < msize; ++note)
  {
    audio.setParameters(win_music[note], 200);
    audio.runBlocking();
  };
};

void draw_cars()
{
  if ((millis() - previousRedraw) > 1000)
  {
    previousRedraw = millis();
    for (byte i = 0; i < Players.Count(); ++i)
    {
      byte j = random(i, Players.Count());
      byte tmp = drawOrder[j];
      drawOrder[j] = drawOrder[i];
      drawOrder[i] = tmp;
    }
  }

  for (byte i = 0; i < Players.Count(); ++i)
  {
    Players[drawOrder[i]].car().Draw(&track);
  }
}

void show_winner(byte winner)
{
  unsigned long raceTime = millis() - raceStartTime;
  int minutes = raceTime / 60000;
  float seconds = (raceTime - (minutes*60000)) / 1000.f;

  Serial.printf("Winner: %s in %02d:%.3f", Players[winner].getName(), minutes, seconds);
  if( checkAndSaveRecord(Players[winner].getName(), raceTime))
  {
    Serial.println(" New Record!");
  }
  
#ifdef LED_CIRCLE
  circleLighting.setParameters(LedLightingCoroutine::THEATER_CHASE, Players[winner].car().getColor(),50);
  //theaterChase(Players[winner].car().getColor(),50);
#endif

  winner_fx(winner);

#ifdef LED_CIRCLE
  delay(3000);
#endif
}

void loop()
{
  audio.runCoroutine();
  trackLighting.runCoroutine();

#ifdef LED_CIRCLE
  circleLighting.runCoroutine();
#endif

  for (word i = 0; i < MaxLED; i++)
  {
    track.setPixelColor(i, track.Color(0, 0, 0));
  };

  for (byte i = 0; i < Obstacles.Count(); ++i)
  {
    Obstacles[i]->Draw(&track);
  }

  if (RaceStarted)
  {
    if (!raceRunning)
    {
      start_race();
    }

    if( StartRaceButton.isPressedThisLoop())
    {
      start_race();
    }
    StartRaceButton.Update();
    EasyModeSwitch.Update();

    for (byte i = 0; i < Players.Count(); ++i)
    {
      Players[i].Update(Obstacles);

      if (Players[i].car().isFinishedRace())
      {
        show_winner(i);
        start_race();
        return;
      }
    }

    Player previousLeader = Players[0];
    Players.Sort();
    if (previousLeader != Players[0])
    {
      Serial.printf("%s overtook %s\n", Players[0].getName(), previousLeader.getName());
#ifdef LED_CIRCLE
      circle.fill(Players[0].car().getColor(),0,MAXLEDCIRCLE);
      circle.show();
#endif
    }

    draw_cars();
  }
  else //RaceStarted==false
  {
    if (raceRunning)
    {
      ResetPlayers();
      Players.Sort();
    }
    raceRunning = false;
  }

  track.show();
}
