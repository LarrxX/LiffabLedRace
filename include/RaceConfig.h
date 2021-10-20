#ifndef RACE_CONFIG
#define RACE_CONFIG

#include <Adafruit_NeoPixel.h>

#include "DynamicArray.h"
#include "DynamicPointerArray.h"
#include "Player.h"
#include "IObstacle.h"

namespace RaceConfig
{
    extern bool RaceStarted;
 
    extern word MaxLoops;
    extern word MaxLED;

    extern DynamicArray<Player> Players;
    extern DynamicPointerArray<IObstacle *> Obstacles;

    extern Adafruit_NeoPixel track;
#ifdef LED_CIRCLE
    extern Adafruit_NeoPixel circle;
#endif

    void ResetTrack();
    void SplitColor( uint32_t color32, uint8_t& r, uint8_t& g, uint8_t& b);
    void ToHTMLColor(uint32_t color32, char html[7]);
    uint32_t FromHTMLColor(const char* color);
};

#endif //RACE_CONFIG