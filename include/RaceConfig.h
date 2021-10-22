#ifndef RACE_CONFIG
#define RACE_CONFIG

#include <Adafruit_NeoPixel.h>

#include "DynamicArray.h"
#include "DynamicPointerArray.h"
#include "Player.h"
#include "IObstacle.h"

namespace RaceConfig
{
    extern bool RaceStarted;
 
    extern word MaxLoops;
    extern word MaxLED;

    extern DynamicArray<Player> Players;
    extern DynamicPointerArray<IObstacle *> Obstacles;

    extern char RecordName[MAX_NAME_LENGTH];
    extern unsigned long RecordTime;

    extern Adafruit_NeoPixel track;
#ifdef LED_CIRCLE
    extern Adafruit_NeoPixel circle;
    
    void colorWipe(uint32_t c, uint8_t wait); 
    void theaterChase(uint32_t c, uint8_t wait); 
    void rainbow(uint8_t wait);

#endif

    void Save();
    void Load();


    bool checkAndSaveRecord(const char* name, unsigned long time);
    void deleteRecord();
};

#endif //RACE_CONFIG