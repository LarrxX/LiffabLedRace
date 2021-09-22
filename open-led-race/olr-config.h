#pragma once

char const softwareId[] = "A4P0";  // A4P -> A = Open LED Race, 4P0 = Game ID (4P = 4 Players, 0=Type 0)
char const version[] = "0.9.6";

enum{
  LEN_UID = 16,
  CFG_VER = 6,  // "5" in V0.9.6 (manage "permanent" param for Box and Slope)
};

#define MAX_CARS 4

#define MAXLED       120 
#define BOXLEN        60 
#define NUMLAP         5 

#define CONTDOWN_PHASE_DURATION  2000 
#define CONTDOWN_STARTSOUND_DURATION  40 

#define NEWRACE_DELAY  5000 

#define PIN_LED        2    // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A
#define PIN_AUDIO      3    // through CAP 2uf to speaker 8 ohms

#define DIG_CONTROL_1  A2 // switch player 1 to PIN and GND
#define DIG_CONTROL_2  A0 // switch player 2 to PIN and GND
#define DIG_CONTROL_3  A3 // switch player 3 to PIN and GND
#define DIG_CONTROL_4  A1 // switch player 4 to PIN and GND

#define COLOR1         track.Color(255,0,0)
#define COLOR2         track.Color(0,255,0)
#define COLOR3         track.Color(0,0,255)
#define COLOR4         track.Color(255,255,255)

#define COLOR_RAMP     track.Color(64,0,64)
#define COLOR_COIN     track.Color(0,255,255)
#define COLOR_BOXMARKS track.Color(64,64,0)
#define COLOR_OIL      track.Color(64,64,0)

#define RAMP_INIT       80
#define RAMP_CENTER     90
#define RAMP_END        100
#define RAMP_HIGH       3
#define RAMP_ALWAYS_ON  false

#define OIL_BEGIN       40
#define OIL_LENGTH      9
#define OIL_ALWAYS_ON   false
#define OIL_PRESS_DELAY 100

#define LED_SEMAPHORE  12 

#define PIN_VCC_ADC1   6  
#define PIN_VCC_ADC2   7  

#define REC_COMMAND_BUFLEN  32  // received command buffer size
#define TX_COMMAND_BUFLEN   80  // send command buffer size
#define EOL            '\n' // End of Command char used in Protocol
