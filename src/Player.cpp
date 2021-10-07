#include "Player.h"

#include "Car.h"
#include "Controller.h"
#include "Defines.h"

byte Player::_ID = 0;

Player::Player(uint32_t carColor, byte controllerPin) : _id(_ID++)
{
    _car = new Car(carColor);
    _controller = new Controller(controllerPin);
}

Player::~Player()
{
    //Do not delete the pointers to allow lazy copy operator
    //This can lead to memory leaks, but in our use case this works
    // delete (_car);
    // _car = NULL;
    // delete (_controller);
    // _controller = NULL;

}

void Player::Reset()
{
    _car->Reset();
    _controller->Reset();
}

void Player::Update()
{
    if (_controller->isPressed() && !_controller->alreadyPressed())
    {
        _car->increaseSpeed(ACEL);
    }
    _car->Update();
    _controller->Update();
}

bool Player::operator<(const Player &other) const
{
    if( _car->getDistance() == other._car->getDistance() )
    {
        return _id < other._id;
    }
    //We want the player with the greatest distance to be first
    return _car->getDistance() > other._car->getDistance();
}
