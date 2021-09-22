#include "SoftTimer.h"

SoftTimer::SoftTimer(unsigned long tout) {
  this->set(tout);
}

SoftTimer::SoftTimer() {
}

/*
 * 
 */
void SoftTimer::set(unsigned long tout) {
  this->timeout=tout;
}


void SoftTimer::start() {
  this->startTime=millis();
}

void SoftTimer::start(unsigned long tout) {
  this->set(tout);
  this->start();
}

unsigned long SoftTimer::time()
{
  return millis() - startTime;
}
/*
 * 
 */
boolean SoftTimer::elapsed(){
  if(this->time() > this->timeout) {
    return(true);
  }
  return(false);
}
