#include "Controller.h"

Controller::Controller(byte pin) : _pin(pin)
{
    pinMode(_pin, INPUT_PULLUP);
    Reset();
}

void Controller::Reset()
{
    _alreadyPressed = false;
}

void Controller::Update()
{
    if (isPressed() && !_alreadyPressed)
    {
        _alreadyPressed = true;
    };
    if (_alreadyPressed && !isPressed())
    {
        _alreadyPressed = false;
    };
}

bool Controller::isPressed() const
{
    return (digitalRead(_pin) == 0);
}