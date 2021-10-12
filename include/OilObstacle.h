#pragma once

#include <IObstacle.h>

class OilObstacle : public IObstacle
{
    public:
    OilObstacle(word start, word end, uint32_t color);
    void Update(Player* player) override;
    void Draw(Adafruit_NeoPixel* led) override;
};
