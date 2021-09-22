#ifndef _OLR_CONTROLLER_LIB_h
#define _OLR_CONTROLLER_LIB_h                   

#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>
#include "SoftTimer.h"
#include "olr-config.h"


enum ctr_type{
    NOT_DEFINED = 0,
    DIGITAL_MODE,
    ANALOG_MODE,
    DEBUG_MODE,
};

typedef struct{
    enum ctr_type mode;
    int pin;
    int adc;
    int badc;
    int delta_analog;
    byte flag_sw;
    SoftTimer timer;
}controller_t;    

void controller_setup( void );

void controller_init( controller_t* ct, enum ctr_type mode, int pin );

byte controller_getStatus( controller_t* ct );

float controller_getSpeed( controller_t* ct );

float controller_getAccel ( void );

bool controller_isActive( int pin );

#endif 
 
