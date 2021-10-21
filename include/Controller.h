#pragma once

#include <Arduino.h>

class Controller
{
protected:
    byte _pin;
    bool _alreadyPressed;
    unsigned long _pressedTime;

public:
    Controller(byte pin);
    bool isPressed() const;

    void Reset();
    void Update();

    bool alreadyPressed() const { return _alreadyPressed; }
    void setAlreadyPressed(bool pressed) { _alreadyPressed = pressed; }

    unsigned long getPressedTime() const { return _pressedTime; }
    byte getPin() const { return _pin; }
};
