#pragma once

#include <IObstacle.h>

class Car;

class RampObstacle : public IObstacle
{
    public:
    enum RampStyle
    {
        RAMP_HILL,
        RAMP_HOLE,
        RAMP_UP,
        RAMP_DOWN,
    } ;

    protected:
    RampStyle _style;
    byte _height;

    public:
    RampObstacle(word start, word end, byte height, uint32_t color, RampStyle style);
    void Update(Player* player) override;
    void Draw(Adafruit_NeoPixel* led) override;

    private:
    void DrawUp(word left, word right, Adafruit_NeoPixel* led);
    void DrawDown(word left, word right, Adafruit_NeoPixel* led);
    float MoveUp(word left, word right, float position);
    float MoveDown(word left, word right, float position);

    static uint32_t ColorMultiply( uint32_t color, float mul);
};
