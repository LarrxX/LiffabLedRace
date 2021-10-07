#pragma once

//#define LED_CIRCLE

#define MAXLED 120      // MAX LEDs actives on strip
#define MAXLEDCIRCLE 24 // MAX LEDs actives on circle

#define MAX_LOOPS 5 //Number of loops in the race

#define PIN_LED 25    // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A
#define PIN_CIRCLE 26 // R 500 ohms to IN pin for WS2812 and WS2813, for WS2813 IN pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A

#define PIN_AUDIO 32 // through CAP 2uf to speaker 8 ohms

#define INI_RAMP 300
#define MED_RAMP 360
#define END_RAMP 420
#define HIGH_RAMP 25

#define MAX_OBSTACLES 2

#define INIT_OBSTACLES                \
    Obstacles.Add(new OilObstacle(100,110,Adafruit_NeoPixel::Color(0, 255, 255))); \
    Obstacles.Add(new RampObstacle(30,50,Adafruit_NeoPixel::Color(255, 0, 255)));

#define MAX_PLAYERS 4
#define PIN_P1 16 // switch player 1 to PIN and GND
#define PIN_P2 17 // switch player 2 to PIN and GND
#define PIN_P3 18 // switch player 3 to PIN and GND
#define PIN_P4 19 // switch player 4 to PIN and GND

#define COLOR_P1 Adafruit_NeoPixel::Color(255, 0, 0)
#define COLOR_P2 Adafruit_NeoPixel::Color(0, 255, 0)
#define COLOR_P3 Adafruit_NeoPixel::Color(0, 0, 255)
#define COLOR_P4 Adafruit_NeoPixel::Color(255, 255, 255)

#define INIT_PLAYERS                   \
    Players.Add(Player(COLOR_P1, PIN_P1)); \
    Players.Add(Player(COLOR_P2, PIN_P2)); \
    Players.Add(Player(COLOR_P3, PIN_P3)); \
    Players.Add(Player(COLOR_P4, PIN_P4));

// Serial Communications
#define EOL '\n' // End of Command char used in Protocol

#define REC_COMMAND_BUFLEN 32
#define TX_COMMAND_BUFLEN 64

#define ACEL 0.2f //Acceleration
#define kf 0.015f //friction constant
#define kg 0.003f //gravity constant

#define DYNAMIC_CHUNK_SIZE 5