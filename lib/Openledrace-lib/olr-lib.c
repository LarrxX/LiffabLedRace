#include "Arduino.h"
#include "olr-lib.h"



void process_main_track( track_t* tck, car_t* car );
void process_aux_track( track_t* tck, car_t* car );

void init_car( car_t* car, controller_t* ct, uint32_t color ) {
  car->ct = ct;
  car->color = color;
  car->trackID = TRACK_MAIN;
  car->speed=0;
  car->dist=0;
  car->dist_aux=0;
}

void update_controller( car_t* car ) {
    car->speed += get_controller( car->ct ); 
}

void update_track( track_t* tck, car_t* car ) {
  controller_t* ct = car->ct;
  struct cfgtrack const* cfg = &tck->cfg.track;

  if ( car->trackID == TRACK_MAIN    
       &&  (int)car->dist % cfg->nled_main == (cfg->init_aux-(cfg->nled_aux)) 
    //  &&  get_controllerStatus( ct ) == 0 ) {              //change track by switch
      &&  (car->speed <= SPD_MIN_TRACK_AUX )) {              //change track by low speed
        
        car->trackID = TRACK_AUX;
        car->dist_aux = 0;
  }
  else if( car->trackID == TRACK_AUX 
      && car->dist_aux > cfg->nled_aux ) {

        car->trackID = TRACK_MAIN;
        car->dist += cfg->nled_aux;         
  }

  /* Update car position in the current track */
  if ( car->trackID == TRACK_AUX )   process_aux_track( tck, car );
  else if ( car->trackID == TRACK_MAIN )  process_main_track( tck, car );

  /* Update car lap */          
  if ( car->dist > ( cfg->nled_main*car->nlap -1) )  car->nlap++;
}

void process_aux_track( track_t* tck, car_t* car ){
    struct cfgtrack const* cfg = &tck->cfg.track;

    if (  (int)car->dist_aux == tck->ledcoin 
          && car->speed <= get_accel() ) {                      
        car->speed = get_accel ()*10;
        tck->ledcoin = COIN_RESET;
    };

    car->speed -= car->speed * cfg->kf;
    car->dist_aux += car->speed;
}


void process_main_track( track_t* tck, car_t* car ) {
    struct cfgtrack const* cfg = &tck->cfg.track;
    
    if ( tck->rampactive ) { 
        struct cfgramp const* r = &tck->cfg.ramp;
        int const pos = (int)car->dist % cfg->nled_main;
        if ( pos >= r->init && pos < r->center )
            car->speed -= cfg->kg * r->high * ( pos - r->init );
        
        if ( pos <= r->end && pos > r->center )
            car->speed += cfg->kg * r->high * ( pos - r->center ); 
    }

    car->speed -= car->speed * cfg->kf;
    car->dist += car->speed;
}

void init_ramp( track_t* tck ) {
  tck->rampactive = true;
}


bool ramp_isactive( track_t* tck ) {
  return tck->rampactive;
}


void reset_carPosition( car_t* car) {

  car->trackID = TRACK_MAIN;
  car->speed = 0;
  car->dist = 0;
  car->dist_aux = 0;
  car->nlap = 1;
  car->leaving = false;
}

int track_configure( track_t* tck, int init_box ) {
  struct cfgtrack* cfg = &tck->cfg.track;
  if( init_box >= cfg->nled_main ) return -1;
  cfg->nled_main = ( init_box == 0 ) ? cfg->nled_total : init_box;
  cfg->nled_aux = ( init_box == 0 ) ? 0 : cfg->nled_total - init_box;
  cfg->init_aux = init_box - 1;
  return 0;
}

int track_cfgramp( track_t* tck, int center, int high ) {
  struct cfgramp* ramp = &tck->cfg.ramp;

  if ( center >= tck->cfg.track.nled_main || center <= 0 ) return -1;
  ramp->center = center;
  ramp->high = high;
  return 0;
}