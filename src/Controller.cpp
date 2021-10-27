#include "Controller.h"

Controller::Controller(byte pin) : _pin(pin)
{
    pinMode(_pin, INPUT_PULLUP);
    Reset();
}

void Controller::Reset()
{
    _alreadyPressed = false;
    _pressedTime = 0;
}

void Controller::Update()
{
    if (isPressedThisLoop())
    {
        _pressedTime = millis();
        _alreadyPressed = true;
    };
    
    if (isReleasedThisLoop())
    {
        _alreadyPressed = false;
    };
}

bool Controller::isPressed() const
{
    return (digitalRead(_pin) == 0);
}

bool Controller::isPressedThisLoop() const
{
    return isPressed() && !_alreadyPressed;
}
    
bool Controller::isReleasedThisLoop() const
{
    return _alreadyPressed && !isPressed();
}