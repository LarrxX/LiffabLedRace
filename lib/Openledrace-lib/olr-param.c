#include "olr-param.h"

void param_setdefault( struct cfgparam* cfg ) {
    cfg->setted      = true;

    cfg->ramp.init   = 90;
    cfg->ramp.center = 100;
    cfg->ramp.end    = 110;
    cfg->ramp.high   = 2;

    cfg->track.nled_total = MAXLED;
    cfg->track.nled_main  = 300; //240
    cfg->track.nled_aux   = 0; //60
    cfg->track.init_aux   = -1;  //239   
    cfg->track.kf         = 0.015;  //friction constant
    cfg->track.kg         = 0.003; //gravity constant  
}




