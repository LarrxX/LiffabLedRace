#pragma once

#define SAVE_FILE_VERSION "1.1.0"

#define LED_CIRCLE

#ifdef ESP32
    #define EEPROM_SIZE 300
    //If this is not defined, will use EEPROM instead
    #define USE_SPIFFS
#endif

#define DEFAULT_LED 1200// MAX LEDs actives on strip
#define MAXLEDCIRCLE 24 // MAX LEDs actives on circle
#define MAXLEDPLAYER 14 // MAX LEDs actives on player button

#define DEFAULT_LOOPS 3 //Number of loops in the race

#define PIN_LED 21    // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A
#define PIN_CIRCLE 17 // R 500 ohms to IN pin for WS2812 and WS2813, for WS2813 IN pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A

#define PIN_START 15 //Optional button to start race
#define PIN_EASY 16 //Optional switch for easy mode

#define PIN_AUDIO 5 // through CAP 2uf to speaker 8 ohms

#define OIL_PRESS_DELAY_DEFAULT 200

#define DEFAULT_OIL_COLOR Adafruit_NeoPixel::Color(255, 255, 0)
#define DEFAULT_RAMP_COLOR Adafruit_NeoPixel::Color(127, 0, 127)

#define INIT_OBSTACLES                                         \
    Obstacles.Add(new OilObstacle(40, 47, DEFAULT_OIL_COLOR)); \
    Obstacles.Add(new RampObstacle(90, 167, 10, DEFAULT_RAMP_COLOR, RampObstacle::RAMP_UP)); \
    Obstacles.Add(new RampObstacle(168, 220, 8, Adafruit_NeoPixel::Color(0, 127, 0), RampObstacle::RAMP_DOWN)); \
    Obstacles.Add(new RampObstacle(330, 361, 9, DEFAULT_RAMP_COLOR, RampObstacle::RAMP_UP)); \
    Obstacles.Add(new RampObstacle(385, 435, 12, DEFAULT_RAMP_COLOR, RampObstacle::RAMP_UP)); \
    Obstacles.Add(new RampObstacle(436, 550, 8, Adafruit_NeoPixel::Color(0, 127, 0), RampObstacle::RAMP_DOWN));

#define MAX_PLAYERS 4
#define MAX_NAME_LENGTH 15
#define PIN_P1 22 // switch player 1 to PIN and GND
#define PIN_P2 23 // switch player 2 to PIN and GND
#define PIN_P3 14 // switch player 3 to PIN and GND
#define PIN_P4 12 // switch player 4 to PIN and GND

#define COLOR_P1 Adafruit_NeoPixel::Color(0, 255, 0)
#define COLOR_P2 Adafruit_NeoPixel::Color(0, 255, 0)
#define COLOR_P3 Adafruit_NeoPixel::Color(0, 0, 255)
#define COLOR_P4 Adafruit_NeoPixel::Color(255, 255, 255)

#define LIGHT_PIN_P1 33 //PIN of the player 1 button lights
#define LIGHT_PIN_P2 -1 //PIN of the player 1 button lights
#define LIGHT_PIN_P3 -1 //PIN of the player 1 button lights
#define LIGHT_PIN_P4 -1 //PIN of the player 1 button lights

#define INIT_PLAYERS                                           \
    Players.Add(Player(COLOR_P1, PIN_P1, LIGHT_PIN_P1, (char *)"Player 1")); \
    Players.Add(Player(COLOR_P2, PIN_P2, LIGHT_PIN_P2, (char *)"Player 2")); \
    Players.Add(Player(COLOR_P3, PIN_P3, LIGHT_PIN_P3, (char *)"Player 3")); \
    Players.Add(Player(COLOR_P4, PIN_P4, LIGHT_PIN_P4, (char *)"Player 4"));

#define ACEL 0.2f //Acceleration
#define KF 0.015f //friction constant
#define KG 0.003f //gravity constant

#define DYNAMIC_CHUNK_SIZE 5