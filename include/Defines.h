#pragma once

//#define LED_CIRCLE

#define MAXLED 120      // MAX LEDs actives on strip
#define MAXLEDCIRCLE 24 // MAX LEDs actives on circle

#define MAX_LOOPS 5 //Number of loops in the race

#define PIN_LED 25    // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A
#define PIN_CIRCLE 26 // R 500 ohms to IN pin for WS2812 and WS2813, for WS2813 IN pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A

#define PIN_AUDIO 32 // through CAP 2uf to speaker 8 ohms

#define OIL_PRESS_DELAY 500

#define INIT_OBSTACLES                \
    Obstacles.Add(new OilObstacle(30,45,Adafruit_NeoPixel::Color(255, 255, 0))); \
    Obstacles.Add(new RampObstacle(100,120,10,Adafruit_NeoPixel::Color(127, 0, 127), RampObstacle::RAMP_HILL));

#define MAX_PLAYERS 4
#define MAX_NAME_LENGTH 15
#define PIN_P1 16 // switch player 1 to PIN and GND
#define PIN_P2 17 // switch player 2 to PIN and GND
#define PIN_P3 18 // switch player 3 to PIN and GND
#define PIN_P4 19 // switch player 4 to PIN and GND

#define COLOR_P1 Adafruit_NeoPixel::Color(255, 0, 0)
#define COLOR_P2 Adafruit_NeoPixel::Color(0, 255, 0)
#define COLOR_P3 Adafruit_NeoPixel::Color(0, 0, 255)
#define COLOR_P4 Adafruit_NeoPixel::Color(255, 255, 255)

#define INIT_PLAYERS  Players.Add(Player(COLOR_P1, PIN_P1, (char*)"Player 1")); \
     Players.Add(Player(COLOR_P2, PIN_P2, (char*)"Player 2")); \
     Players.Add(Player(COLOR_P3, PIN_P3, (char*)"Player 3")); \
     Players.Add(Player(COLOR_P4, PIN_P4, (char*)"Player 4"));

#define ACEL 0.2f //Acceleration
#define KF 0.015f //friction constant
#define KG 0.003f //gravity constant

#define DYNAMIC_CHUNK_SIZE 5