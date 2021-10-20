#include "RaceConfig.h"

#include "Defines.h"

namespace RaceConfig
{
    bool RaceStarted = true;

    word MaxLoops = DEFAULT_LOOPS;
    word MaxLED = DEFAULT_LED;

    DynamicArray<Player> Players(MAX_PLAYERS);
    DynamicPointerArray<IObstacle *> Obstacles(2);

    Adafruit_NeoPixel track = Adafruit_NeoPixel(MaxLED, PIN_LED, NEO_GRB + NEO_KHZ800);

#ifdef LED_CIRCLE
    Adafruit_NeoPixel circle = Adafruit_NeoPixel(MAXLEDCIRCLE, PIN_CIRCLE, NEO_GRB + NEO_KHZ800);
#endif

    void ResetTrack()
    {
        track = Adafruit_NeoPixel(MaxLED, PIN_LED, NEO_GRB + NEO_KHZ800);
    }
};