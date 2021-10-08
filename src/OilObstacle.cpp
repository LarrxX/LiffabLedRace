#include "OilObstacle.h"

#include <Adafruit_NeoPixel.h>

#include "Player.h"
#include "Car.h"


OilObstacle::OilObstacle(word start, word end, uint32_t color) : IObstacle(start, end, color)
{
}

void OilObstacle::Update(Player *player)
{
    float carPos = player->car().getCurrentDistance();
    if (carPos >= _start && carPos <= _end)
    {
        Serial.println("Car is in OIL!");
    }
}

void OilObstacle::Draw(Adafruit_NeoPixel *led)
{
    for (byte i = _start; i <= _end; ++i)
    {
        led->setPixelColor(i, _color);
    };
}