#pragma once

#include "Coroutines/IBlockingCoroutine.h"

class Adafruit_NeoPixel;

class LedLightingCoroutine : public IBlockingCoroutine
{
public:
    enum LightingType
    {
        COLOR_WIPE,
        RAINBOW,
        THEATER_CHASE
    };

protected:
    Adafruit_NeoPixel *_ledStrip;
    uint32_t _color;
    uint32_t _delay;
    LightingType _type;

public:
    LedLightingCoroutine(Adafruit_NeoPixel *ledStrip);
    void setParameters(LightingType type, uint32_t color, uint32_t delay);
    int runCoroutine() override;

private:
    static uint32_t Wheel(byte WheelPos);
};