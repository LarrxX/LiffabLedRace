#ifndef _OLR_CONTROLLER_LIB_h
#define _OLR_CONTROLLER_LIB_h                   

#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>
#include "SoftTimer.h"

#define DIG_CONTROL_1  A2 // switch player 1 to PIN and GND
#define DIG_CONTROL_2  A0 // switch player 2 to PIN and GND
#define DIG_CONTROL_3  A3 // switch player 3 to PIN and GND
#define DIG_CONTROL_4  A1 // switch player 4 to PIN and GND

#define PIN_VCC_ADC1   6  
#define PIN_VCC_ADC2   7  

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
 
