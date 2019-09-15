
#ifndef _OLR_CONTROLLER_LIB_h
#define _OLR_CONTROLLER_LIB_h                   

#ifdef __cplusplus

extern "C"{
#endif

#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>


#define PIN_P1         A2  // switch player 1 to PIN and GND
#define PIN_P2         A3  // switch player 2 to PIN and GND
#define PIN_VCC_ADC1   6   // switch player 1 to PIN and GND
#define PIN_VCC_ADC2   7   // switch player 2 to PIN and GND

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
}controller_t;    

void setup_controller( void );

void init_controller( controller_t* ct, enum ctr_type mode, int pin );

byte get_controllerStatus( controller_t* ct );

float get_controller( controller_t* ct );

float get_accel ( void );


#ifdef __cplusplus
} // extern "C"
#endif

#endif 
 
