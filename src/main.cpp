
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

 
 by gbarbarov@singulardevices.com  for Arduino day Seville 2019 
 https://www.hackster.io/gbarbarov/open-led-race-a0331a
 https://twitter.com/openledrace
 
 
 https://gitlab.com/open-led-race
 https://openledrace.net/open-software/
*/

//version Basic for PCB Rome Edition
// 2 Player , without Boxes Track

char const softwareId[] = "Liffab"; // A2P0: "A"=OpenLEDRace Team, "2P0"=Game ID (2P=2 Players, 0=Type 0 w slope w/ box)
char const version[] = "1.0.0";

#include <ESP32Tone.h>
#include <Adafruit_NeoPixel.h>

#include "Defines.h"
#include "SerialCommunication.h"
#include "Player.h"
#include "Controller.h"
#include "Car.h"

Player Players[MAX_PLAYERS];

bool ENABLE_RAMP = 0;
bool VIEW_RAMP = 0;

int NPIXELS = MAXLED; // leds on track
int cont_print = 0;

int win_music[] = {
    2637, 2637, 0, 2637,
    0, 2093, 2637, 0,
    3136};

byte gravity_map[MAXLED];
byte drawOrder[MAX_PLAYERS];
unsigned long previousRedraw = 0;

int TBEEP = 0;
int FBEEP = 0;
byte SMOTOR = 0;

byte previousLeader = 0;

unsigned long timestamp = 0;

Adafruit_NeoPixel track = Adafruit_NeoPixel(MAXLED, PIN_LED, NEO_GRB + NEO_KHZ800);

#ifdef LED_CIRCLE
Adafruit_NeoPixel circle = Adafruit_NeoPixel(MAXLEDCIRCLE, PIN_CIRCLE, NEO_GRB + NEO_KHZ800);
#endif

int tdelay = 5;

void start_race();
void send_race_phase(int);
void checkSerialCommand();

void set_ramp(word H, word a, word b, word c)
{
  for (int i = 0; i < (b - a); i++)
  {
    gravity_map[a + i] = 127 - i * ((float)H / (b - a));
  };
  gravity_map[b] = 127;
  for (int i = 0; i < (c - b); i++)
  {
    gravity_map[b + i + 1] = 127 + H - i * ((float)H / (c - b));
  };
}

void set_loop(word H, word a, word b, word c)
{
  for (int i = 0; i < (b - a); i++)
  {
    gravity_map[a + i] = 127 - i * ((float)H / (b - a));
  };
  gravity_map[b] = 255;
  for (int i = 0; i < (c - b); i++)
  {
    gravity_map[b + i + 1] = 127 + H - i * ((float)H / (c - b));
  };
}

void setup()
{
  Serial.begin(115200);

  INIT_PLAYERS

  for (byte i = 0; i < NPIXELS; i++)
  {
    gravity_map[i] = 127;
  };

  for (byte i = 0; i < MAX_PLAYERS; ++i)
  {
    drawOrder[i] = i;
  }

  track.begin();

#ifdef LED_CIRCLE
  circle.begin();
  circle.setBrightness(125);
#endif

  if (Players[0].controller().isPressed()) //push switch 1 on reset for activate physics
  {
    ENABLE_RAMP = 1;
    set_ramp(HIGH_RAMP, INI_RAMP, MED_RAMP, END_RAMP);
    for (int i = 0; i < (MED_RAMP - INI_RAMP); i++)
    {
      track.setPixelColor(INI_RAMP + i, track.Color(24 + i * 4, 0, 24 + i * 4));
    };
    for (int i = 0; i < (END_RAMP - MED_RAMP); i++)
    {
      track.setPixelColor(END_RAMP - i, track.Color(24 + i * 4, 0, 24 + i * 4));
    };
    track.show();
    delay(1000);
    tone(PIN_AUDIO, 500);
    delay(500);
    noTone(PIN_AUDIO);
    delay(500);
    if (Players[0].controller().isPressed())
    {
      VIEW_RAMP = 1;
    } // if retain push switch 1 set view ramp
    else
    {
      for (int i = 0; i < NPIXELS; i++)
      {
        track.setPixelColor(i, track.Color(0, 0, 0));
      };
      track.show();
      VIEW_RAMP = 0;
    };
  };

  if (Players[1].controller().isPressed())
  {
    delay(1000);
    tone(PIN_AUDIO, 1000);
    delay(500);
    noTone(PIN_AUDIO);
    delay(500);
    if (Players[1].controller().isPressed())
      SMOTOR = 1;
  } //push switch 2 until a tone beep on reset for activate magic FX  ;-)

  start_race();
}

void start_race()
{
  send_race_phase(4); // Race phase 4: Countdown
  for (byte i = 0; i < MAX_PLAYERS; ++i)
  {
    Players[i].Reset();
  }

  for (byte i = 0; i < NPIXELS; i++)
  {
    track.setPixelColor(i, track.Color(0, 0, 0));
  };

  track.show();

#ifdef LED_CIRCLE
  circle.fill(circle.Color(0, 0, 255), 0, 6);
  circle.fill(circle.Color(0, 255, 0), 6, 6);
  circle.fill(circle.Color(255, 0, 0), 12, 6);
  circle.fill(circle.Color(255, 255, 255), 18, 6);
  circle.show();
#endif

  delay(2000);

  track.setPixelColor(12, track.Color(0, 255, 0));
  track.setPixelColor(11, track.Color(0, 255, 0));
  track.show();

#ifdef LED_CIRCLE
  circle.fill(circle.Color(255, 255, 255), 0, 6);
  circle.fill(circle.Color(0, 0, 255), 6, 6);
  circle.fill(circle.Color(255, 0, 0), 12, 6);
  circle.fill(circle.Color(0, 255, 0), 18, 6);
  circle.show();
#endif

  tone(PIN_AUDIO, 400);
  delay(2000);
  noTone(PIN_AUDIO);

  track.setPixelColor(12, track.Color(0, 0, 0));
  track.setPixelColor(11, track.Color(0, 0, 0));
  track.setPixelColor(10, track.Color(255, 255, 0));
  track.setPixelColor(9, track.Color(255, 255, 0));
  track.show();

#ifdef LED_CIRCLE
  circle.fill(circle.Color(0, 255, 0), 0, 6);
  circle.fill(circle.Color(255, 255, 255), 6, 6);
  circle.fill(circle.Color(0, 0, 255), 12, 6);
  circle.fill(circle.Color(255, 0, 0), 18, 6);
  circle.show();
#endif

  tone(PIN_AUDIO, 600);
  delay(2000);
  noTone(PIN_AUDIO);

  track.setPixelColor(9, track.Color(0, 0, 0));
  track.setPixelColor(10, track.Color(0, 0, 0));
  track.setPixelColor(8, track.Color(255, 0, 0));
  track.setPixelColor(7, track.Color(255, 0, 0));
  track.show();

  tone(PIN_AUDIO, 1200);
  delay(2000);
  noTone(PIN_AUDIO);

  timestamp = 0;
  previousLeader = 0;
  send_race_phase(5); // Race phase 4: Race Started
};

void winner_fx(byte w)
{
  int msize = sizeof(win_music) / sizeof(int);
  for (int note = 0; note < msize; note++)
  {
    if (SMOTOR == 1)
    {
      tone(PIN_AUDIO, win_music[note] / (3 - w), 200);
    }
    else
    {
      tone(PIN_AUDIO, win_music[note], 200);
    };
    delay(230);
    noTone(PIN_AUDIO);
  }
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
  SerialCommunication::instance().SendCommand("W%d%c", winner, EOL);
#ifdef LED_CIRCLE
  for (int i = 0; i < MAXLEDCIRCLE; i++)
  {
    circle.setPixelColor(i, Players[i].car().getColor());
  };
  circle.show();
#endif

  winner_fx(winner);
}

void loop()
{
  // look for commands received on serial
  //checkSerialCommand();

  for (int i = 0; i < NPIXELS; i++)
  {
    track.setPixelColor(i, track.Color(0, 0, 0));
  };
  if ((ENABLE_RAMP == 1) && (VIEW_RAMP == 1))
  {
    for (int i = 0; i < (MED_RAMP - INI_RAMP); i++)
    {
      track.setPixelColor(INI_RAMP + i, track.Color(24 + i * 4, 0, 24 + i * 4));
    };
    for (int i = 0; i < (END_RAMP - MED_RAMP); i++)
    {
      track.setPixelColor(END_RAMP - i, track.Color(24 + i * 4, 0, 24 + i * 4));
    };
  };

  float maxDist = 0.f;
  byte currentLeader = 0;
  for (byte i = 0; i < MAX_PLAYERS; ++i)
  {
    Players[i].Update();

    if (Players[i].car().isFinishedRace())
    {
      show_winner(i);
      start_race();
      return;
    }
    else
    {
      if (Players[i].car().getDistance() > maxDist)
      {
        maxDist = Players[i].car().getDistance();
        currentLeader = i;
      }

      if (Players[i].car().isStartingNewLoop())
      {
        FBEEP = 440 * (currentLeader + 1);
        TBEEP = 10;
      }
    }
  }

  //Beep when someone new takes the lead
  if (previousLeader != currentLeader)
  {
    FBEEP = 440 * (currentLeader + 1);
    TBEEP = 10;
    previousLeader = currentLeader;
  }

  draw_cars();

  track.show();

  // if (SMOTOR == 1)
  //   tone(PIN_AUDIO, FBEEP + int(speed1 * 440 * 2) + int(speed2 * 440 * 3));

  delay(tdelay);

  if (TBEEP > 0)
  {
    TBEEP--;
  }
  else
  {
    FBEEP = 0;
  };
  // cont_print++;
  // if (cont_print > 100)
  // {
  //   print_cars_position();
  //   cont_print = 0;
  // }
}

/*
 * 
 */

void checkSerialCommand()
{
  int clen = 0;
  const char *cmd = SerialCommunication::instance().ReadSerial(clen);

  if (clen == 0)
    return; // No commands received
  if (clen < 0)
  {                                                                                           // Error receiving command
    SerialCommunication::instance().SendCommand("!1Error reading serial command:[%d]", clen); // Send a warning to host
    return;
  }
  // clen > 0 ---> Command with length=clen ready in  cmd[]

  switch (cmd[0])
  {
  case '#': // Handshake -> send back
  {
    SerialCommunication::instance().SendCommand("#%c", EOL);
  }
    return;

  case '@': // Enter "Configuration Mode"
  {
    // send back @OK
    // No real cfg mode here, but send @OK so the Desktop app (Upload, configure)
    // can send a GET SOFTWARE Type/Ver command and identify this software
    SerialCommunication::instance().SendCommand("@OK%c", EOL);
  }
    return;

  case '?': // Get Software Id
  {
    SerialCommunication::instance().SendCommand("%s%s%c", "?", softwareId, EOL);
  }
    return;

  case '%': // Get Software Version
  {
    SerialCommunication::instance().SendCommand("%s%s%c", "%", version, EOL);
  }
    return;
  }

  // if we get here, the command it's not managed by this software -> Answer <CommandId>NOK
  SerialCommunication::instance().SendCommand("%cNOK%c", cmd[0], EOL);

  return;
}

/*
 * 
 */
void send_race_phase(int phase)
{
  SerialCommunication::instance().SendCommand("R%d%c", phase, EOL);
}
