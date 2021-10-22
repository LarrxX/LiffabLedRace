
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

#ifdef ESP32
#include <ESP32Tone.h>
#endif

#ifdef USE_SPIFFS
#include <SPIFFS.h>
#else
#include <EEPROM.h>
#endif

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

word TBEEP = 0;
word FBEEP = 0;
byte SMOTOR = 0;

void ResetPlayers()
{
  for (byte i = 0; i < Players.Count(); ++i)
  {
    Players[i].Reset();
  }
}

void start_race()
{
  Serial.println("start_race");
  raceRunning = true;
  Serial.println("start_resetplayer");
  ResetPlayers();

  Serial.println("start_resettrack");
  for (word i = 0; i < MaxLED; i++)
  {
    track.setPixelColor(i, track.Color(0, 0, 0));
  };

  Serial.println("start_obstacles");
  for (byte i = 0; i < Obstacles.Count(); ++i)
  {
    Obstacles[i]->Draw(&track);
  }
  track.show();

  #ifdef LED_CIRCLE
    Serial.println("Arc en ciel");
    rainbow(3);
    circle.show();
    circle.fill(circle.Color(255,0,0),0,24);
    circle.show();
  #endif


  tone(PIN_AUDIO, 400);
  delay(2000);
  noTone(PIN_AUDIO);

  #ifdef LED_CIRCLE
    circle.fill(circle.Color(255,0,0),0,24);
    circle.show();
  #endif

  tone(PIN_AUDIO, 600);
  delay(2000);
  noTone(PIN_AUDIO);

  #ifdef LED_CIRCLE
    circle.fill(circle.Color(255,132,0),0,24);
    circle.show();
  #endif

  tone(PIN_AUDIO, 1200);
  delay(2000);
  noTone(PIN_AUDIO);
  raceStartTime = millis();

#ifdef LED_CIRCLE
    circle.fill(circle.Color(0,255,0),0,24);
    circle.show();
#endif

  Serial.println("race_started");
}

void setup()
{
  raceRunning = false;

  INIT_PLAYERS
  INIT_OBSTACLES
  Obstacles.Sort();

  WebService::Instance().Init();

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

  for (byte i = 0; i < MAX_PLAYERS; ++i)
  {
    drawOrder[i] = i;
  }

  track.begin();

#ifdef LED_CIRCLE
  circle.begin();
  circle.setBrightness(125);
#endif

  if (Players[0].controller().isPressed())
  {
    delay(1000);
    tone(PIN_AUDIO, 1000);
    delay(500);
    noTone(PIN_AUDIO);
    delay(500);
    if (Players[1].controller().isPressed())
      SMOTOR = 1;
  } //push switch 1 until a tone beep on reset for activate magic FX  ;-)

  start_race();
}

void winner_fx(byte w)
{
  word msize = sizeof(win_music) / sizeof(word);
  for (word note = 0; note < msize; ++note)
  {
    tone(PIN_AUDIO, win_music[note], 200);
  };
  delay(230);
  noTone(PIN_AUDIO);
};

void draw_cars()
{
  if ((millis() - previousRedraw) > 1000)
  {
    previousRedraw = millis();
    for (byte i = 0; i < MAX_PLAYERS; ++i)
    {
      byte j = random(i, MAX_PLAYERS);
      byte tmp = drawOrder[j];
      drawOrder[j] = drawOrder[i];
      drawOrder[i] = tmp;
    }
  }

  for (byte i = 0; i < MAX_PLAYERS; ++i)
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
  theaterChase(Players[winner].car().getColor(),50);
#endif

  winner_fx(winner);
}

void loop()
{
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

    for (byte i = 0; i < Players.Count(); ++i)
    {
      Players[i].Update(Obstacles);

      if (Players[i].car().isFinishedRace())
      {
        show_winner(i);
        start_race();
        return;
      }

      if (Players[i].car().isStartingNewLoop())
      {
        FBEEP = 440 * (i + 1);
        TBEEP = 10;
      }
    }

    Player previousLeader = Players[0];
    Players.Sort();
    //Beep when someone new takes the lead
    if (previousLeader != Players[0])
    {
      Serial.printf("%s overtook %s\n", Players[0].getName(), previousLeader.getName());
#ifdef LED_CIRCLE
      circle.fill(Players[0].car().getColor(),0,24);
      circle.show();
#endif
      FBEEP = 440;
      TBEEP = 10;
    }

    // if (SMOTOR == 1)
    //   tone(PIN_AUDIO, FBEEP + word(speed1 * 440 * 2) + word(speed2 * 440 * 3));

    draw_cars();

    if (TBEEP > 0)
    {
      TBEEP--;
    }
    else
    {
      FBEEP = 0;
    };
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
