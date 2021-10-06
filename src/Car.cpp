#include "Car.h"

#include <Adafruit_NeoPixel.h>

#include "Defines.h"

Car::Car(uint32_t color) : _color(color)
{
  Reset();
}

void Car::Reset()
{
  _speed = 0.f;
  _distance = 0.f;
  _currentLoop = 1;
  _newLoopStarted = false;
  _finishedRace = false;
}

void Car::Update()
{
  _newLoopStarted = false;
  //   if ((gravity_map[(word)dist1 % NPIXELS]) < 127)
  //     speed1 -= kg * (127 - (gravity_map[(word)dist1 % NPIXELS]));
  //   if ((gravity_map[(word)dist1 % NPIXELS]) > 127)
  //     speed1 += kg * ((gravity_map[(word)dist1 % NPIXELS]) - 127);

  _speed -= _speed * kf; //friction

  _distance += _speed;

  if (_distance > _currentLoop * MAXLED)
  {
    _newLoopStarted = true;
    ++_currentLoop;
    if (_currentLoop == MAX_LOOPS)
    {
      _finishedRace = true;
    }
  }
}

void Car::Draw(Adafruit_NeoPixel *led) const
{
  for (byte i = 0; i <= _currentLoop; ++i)
  {
    led->setPixelColor(((word)_distance % MAXLED) + i, _color);
  };
}