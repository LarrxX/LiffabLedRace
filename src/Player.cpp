#include "Player.h"

#include "Car.h"
#include "Controller.h"

Player::Player() : _isInit(false),
                   _car(NULL),
                   _controller(NULL)
{
}

Player::~Player()
{
    if (isInit())
    {
        delete (_car);
        _car = NULL;
        delete (_controller);
        _controller = NULL;
    }
}

void Player::Init(uint32_t carColor, byte controllerPin)
{
    _car = new Car(carColor);
    _controller = new Controller( controllerPin );
    _isInit = true;
}