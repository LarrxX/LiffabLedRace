#include "Player.h"

#include "Car.h"
#include "Controller.h"
#include "Defines.h"

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
    if (!isInit())
    {
        _car = new Car(carColor);
        _controller = new Controller(controllerPin);
        _isInit = true;
    }
}

void Player::Reset()
{
    if (isInit())
    {
        _car->Reset();
        _controller->Reset();
    }
}

void Player::Update()
{
    if (isInit())
    {
        if( _controller->isPressed() && !_controller->alreadyPressed() )
        {
            _car->Update();
        }
        _controller->Update();
    }
}
