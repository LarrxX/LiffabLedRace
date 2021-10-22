#include "RaceConfig.h"

#include <EEPROM.h>

#include "Defines.h"
#include "Car.h"
#include "Controller.h"
#include "OilObstacle.h"
#include "RampObstacle.h"

namespace RaceConfig
{
    bool RaceStarted = true;

    word MaxLoops = DEFAULT_LOOPS;
    word MaxLED = DEFAULT_LED;

    DynamicArray<Player> Players(MAX_PLAYERS);
    DynamicPointerArray<IObstacle *> Obstacles(2);

    Adafruit_NeoPixel track = Adafruit_NeoPixel(MaxLED, PIN_LED, NEO_GRB + NEO_KHZ800);

#ifdef LED_CIRCLE
    Adafruit_NeoPixel circle = Adafruit_NeoPixel(MAXLEDCIRCLE, PIN_CIRCLE, NEO_GRB + NEO_KHZ800);
#endif

    void Save()
    {
        int eeAddress = 0;

        EEPROM.writeInt(eeAddress, MaxLoops);
        eeAddress += sizeof(MaxLoops);

        EEPROM.writeInt(eeAddress, MaxLED);
        eeAddress += sizeof(MaxLED);

        size_t count = Players.Count();
        EEPROM.writeInt(eeAddress, count);
        eeAddress += sizeof(count);

        for (size_t i = 0; i < count; ++i)
        {
            Player &player = Players[i];

            EEPROM.writeUInt(eeAddress, player.car().getColor());
            eeAddress += sizeof(uint32_t);

            EEPROM.writeByte(eeAddress, player.controller().getPin());
            eeAddress += sizeof(byte);

            EEPROM.writeBytes(eeAddress, player.getName(), strlen(player.getName()) + 1);
            eeAddress += (strlen(player.getName()) + 1) * sizeof(char);
        }

        count = Obstacles.Count();
        EEPROM.writeInt(eeAddress, count);
        eeAddress += sizeof(count);

        for (size_t i = 0; i < count; ++i)
        {
            IObstacle* obstacle = Obstacles[i];

            EEPROM.writeUInt(eeAddress, obstacle->getColor());
            eeAddress += sizeof(uint32_t);

            EEPROM.writeInt(eeAddress, obstacle->getStart());
            eeAddress += sizeof(word);

            EEPROM.writeInt(eeAddress, obstacle->getEnd());
            eeAddress += sizeof(word);

            EEPROM.writeInt( eeAddress, obstacle->getType());
            eeAddress += sizeof(IObstacle::ObstacleType);

            switch(obstacle->getType())
            {
                case IObstacle::OBSTACLE_OIL:
                {
                    OilObstacle* oil = static_cast<OilObstacle*>(obstacle);
                    EEPROM.writeInt(eeAddress, oil->getPressDelay());
                    eeAddress += sizeof(word);
                }
                break;

                case IObstacle::OBSTACLE_RAMP:
                {
                    RampObstacle* ramp = static_cast<RampObstacle*>(obstacle);
                    
                    EEPROM.writeByte(eeAddress, ramp->getHeight());
                    eeAddress += sizeof(byte);

                    EEPROM.writeInt(eeAddress, ramp->getStyle());
                    eeAddress += sizeof(RampObstacle::RampStyle);
                }
                break;
            }
        }
        EEPROM.commit();
    }

    void Load()
    {
        Serial.println("Loading...");
        int eeAddress = 0;

        EEPROM.get(eeAddress, MaxLoops);
        eeAddress += sizeof(MaxLoops);
        Serial.printf("Loaded Maxloops=%d\n", MaxLoops);

        EEPROM.get(eeAddress, MaxLED);
        eeAddress += sizeof(MaxLED);
        Serial.printf("Loaded Maxloops=%d\n", MaxLED);
        track.updateLength(MaxLED);
        Serial.println("Track length updated");

        size_t count;
        EEPROM.get(eeAddress, count);
        eeAddress += sizeof(count);
        Serial.printf("%d player count\n", count);

        //Explicitly destroy current players contents to remove all internal pointers
        //This avoids memory leaks since the ponters are not automatically destroyed in ~Player() by design
        for( size_t i = 0; i < Players.Count(); ++i )
        {
            Serial.printf("Destroying player %d\n", i);
            Players[i].Destroy();
        }
        Players.Clear();

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Loading player %d:\n", i);

            uint32_t color = 0;
            EEPROM.get(eeAddress, color);
            eeAddress += sizeof(color);
            Serial.printf("Color=%d", color);

            byte pin = 0;
            EEPROM.get(eeAddress, pin);
            eeAddress += sizeof(pin);
            Serial.printf(" Pin=%d", pin);

            char name[MAX_NAME_LENGTH];
            EEPROM.get(eeAddress, name);
            eeAddress += (strlen(name) + 1) * sizeof(char);
            Serial.printf(" Name=%s\n", name);

            Players.Add(Player(color, pin, name));
            Serial.println("Player loaded!");
        }

        EEPROM.get(eeAddress, count);
        eeAddress += sizeof(count);
        Serial.printf("%d obstacle count\n", count);

        Obstacles.Clear();

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Loading obstacle %d:\n", i);

            uint32_t color = 0;
            EEPROM.get(eeAddress, color);
            eeAddress += sizeof(color);
            Serial.printf("Color=%d", color);

            word start = 0;
            EEPROM.get(eeAddress, start);
            eeAddress += sizeof(start);
            Serial.printf(" Start=%d", start);

            word end = 0;
            EEPROM.get(eeAddress, end);
            eeAddress += sizeof(end);
            Serial.printf(" End=%d", end);

            IObstacle::ObstacleType type;
            EEPROM.get(eeAddress, type);
            eeAddress += sizeof(type);
            Serial.printf(" Type=%d", type);

            switch(type)
            {
                case IObstacle::OBSTACLE_OIL:
                {
                    word pressDelay = 0;
                    EEPROM.get(eeAddress, pressDelay);
                    eeAddress += sizeof(pressDelay);
                    Serial.printf(" Delay=%d\n", pressDelay);

                    OilObstacle* oil = new OilObstacle(start, end, color);
                    Serial.println("Oil obstacle created");
                    oil->setPressDelay(pressDelay);

                    Obstacles.Add(oil);
                    Serial.println("Oil obstacle loaded!");
                }
                break;

                case IObstacle::OBSTACLE_RAMP:
                {
                    byte height = 0;
                    EEPROM.get(eeAddress, height);
                    eeAddress += sizeof(height);
                    Serial.printf(" height=%d", height);

                    RampObstacle::RampStyle style;
                    EEPROM.get(eeAddress, style);
                    eeAddress += sizeof(style);
                    Serial.printf(" style=%d\n", style);

                    RampObstacle* ramp = new RampObstacle(start, end, height, color, style);
                    Serial.println("Ramp obstacle created");

                    Obstacles.Add(ramp);
                    Serial.println("Ramp obstacle loaded!");
                }
                break;
            }
        }
        Serial.println("Loading done!");
    }
};