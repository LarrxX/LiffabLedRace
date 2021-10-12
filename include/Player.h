#pragma once

#include <Arduino.h>

class Car;
class Controller;
class IObstacle;

template <class T>
class DynamicPointerArray;

class Player
{
    protected:
    Car *_car;
    Controller *_controller;
    byte _currentObstacle;
    
    static byte _ID;
    byte _id;


    public:
    Player(uint32_t carColor, byte controllerPin);
    ~Player();

    void Update(DynamicPointerArray<IObstacle*>& obstacles);
    void Reset();
    
    const Controller& controller() const { return *_controller; }
    const Car& car() const { return *_car; }
    Car& mutableCar() { return *_car; }
    
    byte id() const { return _id;}

    bool operator<(const Player& other) const;
    
    private:
    Player();
};
