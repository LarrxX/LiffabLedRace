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
    
    const bool& isInit(){ return _isInit; }
};
