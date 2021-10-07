#pragma once

#include <Arduino.h>

class Car;
class Adafruit_NeoPixel;

class IObstacle
{
public:
    word _start;
    word _end;
    uint32_t _color;

public:
    IObstacle(word start, word end, uint32_t color);
    virtual void Update(Car* car) = 0;
    virtual void Draw(Adafruit_NeoPixel* led) = 0;

    word getEnd() const { return _end; };

    bool operator<(const IObstacle& other)
    {
        if( _start == other._start)
        {
            return _end <= other._end;
        }
        
        return _start < other._start;
    }
};
