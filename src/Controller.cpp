#include "Controller.h"

Controller::Controller(byte pin) : _pin(pin),
                                   _alreadyPressed(false)
{
    pinMode(_pin, INPUT_PULLUP);
}

bool Controller::isPressed()
{
    return (digitalRead(_pin) == 0);
}