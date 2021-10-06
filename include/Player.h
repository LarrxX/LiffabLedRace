#pragma once

#include <Arduino.h>

class Car;
class Controller;

class Player
{
    protected:
    bool _isInit;
    Car *_car;
    Controller *_controller;
    

    public:
    Player();
    ~Player();
    void Init(uint32_t carColor, byte controllerPin);

    void Update();
    void Reset();
            
    const bool& isInit() const{ return _isInit; }
    const Controller& controller() const { return *_controller; }
    const Car& car() const { return *_car; }
};
