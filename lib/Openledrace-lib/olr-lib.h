
#ifndef _OLR_LIB_h
#define _OLR_LIB_h                   

#ifdef __cplusplus

extern "C"{
#endif

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "olr-controller.h"
#include "olr-param.h"

enum{
  NOT_TRACK = 0,
  TRACK_MAIN,
  TRACK_AUX,
  TRACK_IN,
  TRACK_OUT,
  NUM_TRACKS,
};

  

enum status{
    CAR_WAITING = 0,
    CAR_COMING,
    CAR_ENTER,
    CAR_RACING,
    CAR_LEAVING,
    CAR_GO_OUT,
    CAR_FINISH
};

typedef struct{
    controller_t* ct;
    float speed;
    float dist;
    float dist_aux;
    byte nlap;
    byte repeats;
    uint32_t color;
    int trackID;
    enum status st;
    bool leaving;
}car_t;



typedef struct {
  struct cfgparam cfg;
  int led_speed; //LED_SPEED_COIN
  byte* gmap; //pointer to gravity map
}track_t;


void init_ramp( track_t* tck );

void set_ramp( track_t* tck );

void init_car( car_t* car, controller_t* ct, uint32_t color );

void update_track( track_t* tck, car_t* car );

void update_controller( car_t* car );

void reset_carPosition( car_t* car);

int track_configure( track_t* tck, int init_box );

int track_cfgramp( track_t* tck, int center, int high );

#ifdef __cplusplus
} // extern "C"
#endif

#endif

