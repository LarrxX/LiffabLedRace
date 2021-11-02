#include "Coroutines/LedLighting.h"

#include <Adafruit_NeoPixel.h>

LedLightingCoroutine::LedLightingCoroutine(Adafruit_NeoPixel *ledStrip) : _ledStrip(ledStrip),
                                                                          _color(0),
                                                                          _delay(0),
                                                                          _type(COLOR_WIPE)
{
}

void LedLightingCoroutine::setParameters(LightingType type, uint32_t color, uint32_t delay)
{
    reset();
    _type = type;
    _color = color;
    _delay = delay;
}

int LedLightingCoroutine::runCoroutine()
{
    COROUTINE_BEGIN();
    switch (_type)
    {
    // Fill the LEDs one after the other with a color
    case COLOR_WIPE:
    {
        for (int i = 0; i < _ledStrip->numPixels(); ++i)
        {
            _ledStrip->setPixelColor(i, _color);
            _ledStrip->show();
            BLOCKING_COROUTINE_DELAY(_delay);
        }
    }
    break;

    //Cycle through all colors of the rainbow
    case RAINBOW:
    {
        for (int j = 0; j < 256; ++j)
        {
            for (int i = 0; i < _ledStrip->numPixels(); ++i)
            {
                _ledStrip->setPixelColor(i, Wheel((i * 1 + j) & 255));
            }
            _ledStrip->show();
            BLOCKING_COROUTINE_DELAY(_delay);
        }
    }
    break;

    //Theatre-style crawling lights.
    case THEATER_CHASE:
    {
        for (int j = 0; j < 10; ++j)
        { //do 10 cycles of chasing
            for (static int q = 0; q < 3; ++q)
            {
                for (int i = 0; i < _ledStrip->numPixels(); i += 3)
                {
                    //turn every third pixel on
                    _ledStrip->setPixelColor(i + q, _color);
                }
                _ledStrip->show();
                BLOCKING_COROUTINE_DELAY(_delay);
                for (int i = 0; i < _ledStrip->numPixels(); i += 3)
                {
                    //turn every third pixel off
                    _ledStrip->setPixelColor(i + q, 0);
                }
            }
        }
    }
    break;
    }

    COROUTINE_END();
}

uint32_t LedLightingCoroutine::Wheel(byte WheelPos)
{
    if (WheelPos < 85)
    {
        return Adafruit_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    }
    else if (WheelPos < 170)
    {
        WheelPos -= 85;
        return Adafruit_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    else
    {
        WheelPos -= 170;
        return Adafruit_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}
