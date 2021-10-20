#ifndef RACE_CONFIG
#define RACE_CONFIG

#include <Adafruit_NeoPixel.h>

#include "DynamicArray.h"
#include "DynamicPointerArray.h"
#include "Player.h"
#include "IObstacle.h"

namespace RaceConfig
{
    enum State
    {
        RACE_STOPPED,
        RACE_STARTED,
        RACE_CONFIGURING
    };

    extern State RaceState;
 
    extern word MaxLoops;
    extern word MaxLED;

    extern DynamicArray<Player> Players;
    extern DynamicPointerArray<IObstacle *> Obstacles;

    extern Adafruit_NeoPixel track;

    void ResetTrack();
#ifdef LED_CIRCLE
    extern Adafruit_NeoPixel circle;
#endif
};

#endif //RACE_CONFIG