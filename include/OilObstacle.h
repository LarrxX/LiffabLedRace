#pragma once

#include <IObstacle.h>

class OilObstacle : public IObstacle
{
    public:
    OilObstacle(word start, word end, uint32_t color);
    void Update(Car* car) override;
    void Draw(Adafruit_NeoPixel* led) override;
};
