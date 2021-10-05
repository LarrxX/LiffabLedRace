#pragma once

#include <Arduino.h>

class Controller
{
    protected:
    byte _pin;
    bool _alreadyPressed; //flag_sw

    public:
    Controller(byte pin);

    bool isPressed();
};
