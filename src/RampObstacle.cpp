#include "RampObstacle.h"

#include "Car.h"
#include <Adafruit_NeoPixel.h>

RampObstacle::RampObstacle(word start, word end, uint32_t color) : IObstacle(start, end, color)
{
}

void RampObstacle::Update(Car *car)
{
    float carPos = car->getDistance();
    if (carPos >= _start && carPos <= _end)
    {
        Serial.println("Car is on RAMP!");
    }
}

void RampObstacle::Draw(Adafruit_NeoPixel *led)
{
    for (byte i = _start; i <= _end; ++i)
    {
        led->setPixelColor(i, _color);
    };
}