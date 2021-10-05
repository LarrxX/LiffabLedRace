#pragma once

#include <Arduino.h>

class Car
{
    protected:
    uint32_t _color;
    float _speed;
    float _distance;
    byte _currentLoop;

    public:
    Car(uint32_t color);
};
