#pragma once

#include <Arduino.h>

#include "Defines.h"

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
    char _name[MAX_NAME_LENGTH];

public:
    Player(uint32_t carColor, byte controllerPin, char *name);
    ~Player();

    void Update(DynamicPointerArray<IObstacle *> &obstacles);
    void Reset();

    const Controller &controller() const { return *_controller; }
    const Car &car() const { return *_car; }
    Car &mutableCar() { return *_car; }

    const char *getName() const { return _name; }
    void setName(char *name);

    bool operator<(const Player &other) const;
    bool operator==(const Player &other) const;
    bool operator!=(const Player &other) const;

private:
    Player();
};
