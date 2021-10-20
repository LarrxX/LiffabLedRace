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

    void SplitColor(uint32_t color32, uint8_t &r, uint8_t &g, uint8_t &b)
    {
        r = ((color32 >> 16) & 0xFF);
        g = ((color32 >> 8) & 0xFF);
        b = (color32 & 0xFF);
    }
};