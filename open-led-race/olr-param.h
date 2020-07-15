
#ifndef _OLR_SERIAL_LIB_h
#define _OLR_SERIAL_LIB_h                   

#ifdef __cplusplus

extern "C"{
#endif

#include "Arduino.h"
#include <stdint.h>
#include <stdbool.h>

#define MAXLED     240+60  // 466 MAX LEDs actives on strip

enum{
  LEN_UID = 16,
  CFG_VER = 3,
};


// ramp centred in LED 100 with 10 led fordward and 10 backguard
struct cfgtrack  {
  int nled_total;
  int nled_main;
  int nled_aux;
  int init_aux;
  float kf;
  float kg;
};

// ramp centred in LED 100 with 10 led fordward and 10 backguard
struct cfgramp  {
  int init;
  int center;
  int end;
  int high;
};

struct brdinfo {
  char uid[LEN_UID + 1];
};

struct cfgparam {
    bool setted;
    struct cfgtrack track;
    struct cfgramp  ramp;
    struct brdinfo  info;
};


void param_setdefault( struct cfgparam* cfg );

#ifdef __cplusplus
} // extern "C"
#endif

#endif 
 
