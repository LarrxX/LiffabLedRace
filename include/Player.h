#pragma once

#include <Arduino.h>

class Car;
class Controller;

class Player
{
    protected:
    Car *_car;
    Controller *_controller;
    static byte _ID;
    byte _id;

    public:
    Player(uint32_t carColor, byte controllerPin);
    ~Player();

    void Update();
    void Reset();
    
    const Controller& controller() const { return *_controller; }
    const Car& car() const { return *_car; }
    byte id() const { return _id;}

    bool operator<(const Player& other) const;
    
    private:
    Player();
};
