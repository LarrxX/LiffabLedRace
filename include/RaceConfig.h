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
};

#endif //RACE_CONFIG