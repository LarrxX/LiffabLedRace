#pragma once

#include <IObstacle.h>

class RampObstacle : public IObstacle
{
    public:
    RampObstacle(word start, word end, uint32_t color);
    void Update(Car* car) override;
    void Draw(Adafruit_NeoPixel* led) override;
};
