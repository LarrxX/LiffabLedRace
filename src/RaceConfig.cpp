#include "RaceConfig.h"

#ifdef USE_SPIFFS
#include <SPIFFS.h>
#else
#include <EEPROM.h>
#endif

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

    void writeWord(int &offset, word data)
    {
        EEPROM.writeUInt(offset, data);
        offset += sizeof(int);
    }

    void readWord(int &offset, word &data)
    {
        EEPROM.get(offset, data);
        offset += sizeof(word);
    }

    void writeUInt(int &offset, uint32_t data)
    {
        writeWord(offset, (word)data);
    }

    void readUInt(int &offset, uint32_t& data)
    {
        word wordData;
        readWord(offset, wordData);
        data = wordData;
    }

    void writeByte(int &offset, byte data)
    {
        EEPROM.writeByte(offset, data);
        offset += sizeof(byte);
    }

    void readByte(int &offset, byte& data)
    {
        EEPROM.get(offset, data);
        offset += sizeof(byte);
    }

    void writeString(int &offset, const char *data)
    {
        size_t len = strlen(data) + 1;
        
        Serial.printf("Writing string %s of length %d\n", data, len);

        EEPROM.writeBytes(offset, data, len);
        offset += len * sizeof(char);
    }

    void readString(int &offset, byte* data)
    {
        for( int i = 0; i < MAX_NAME_LENGTH; ++i)
        {
            readByte(offset, data[i]);
            if( data[i] =='\0')
            {
                return;
            }
        }
    }

    void Save()
    {
        int eeAddress = 0;

        writeWord(eeAddress, MaxLoops);

        writeWord(eeAddress, MaxLED);

        size_t count = Players.Count();
        writeWord(eeAddress, count);

        for (size_t i = 0; i < count; ++i)
        {
            Player &player = Players[i];

            writeUInt(eeAddress, player.car().getColor());

            writeByte(eeAddress, player.controller().getPin());

            writeString(eeAddress, player.getName());
        }

        count = Obstacles.Count();
        writeWord(eeAddress, count);

        for (size_t i = 0; i < count; ++i)
        {
            IObstacle *obstacle = Obstacles[i];

            writeUInt(eeAddress, obstacle->getColor());

            writeWord(eeAddress, obstacle->getStart());

            writeWord(eeAddress, obstacle->getEnd());

            writeWord(eeAddress, obstacle->getType());

            switch (obstacle->getType())
            {
            case IObstacle::OBSTACLE_OIL:
            {
                OilObstacle *oil = static_cast<OilObstacle *>(obstacle);
                writeWord(eeAddress, oil->getPressDelay());
            }
            break;

            case IObstacle::OBSTACLE_RAMP:
            {
                RampObstacle *ramp = static_cast<RampObstacle *>(obstacle);

                writeByte(eeAddress, ramp->getHeight());

                writeWord(eeAddress, ramp->getStyle());
            }
            break;
            }
        }
#if defined(ESP32) && !defined(USE_SPIFFS)
        EEPROM.commit();
#endif
    }

    void Load()
    {
        Serial.println("Loading...");
        int eeAddress = 0;

        readWord(eeAddress, MaxLoops);
        Serial.printf("Loaded Maxloops=%d\n", MaxLoops);

        readWord(eeAddress, MaxLED);
        Serial.printf("Loaded Maxloops=%d\n", MaxLED);
        track.updateLength(MaxLED);
        Serial.println("Track length updated");

        word count;
        readWord(eeAddress, count);
        Serial.printf("%d player count\n", count);

        //Explicitly destroy current players contents to remove all internal pointers
        //This avoids memory leaks since the ponters are not automatically destroyed in ~Player() by design
        for (int i = 0; i < Players.Count(); ++i)
        {
            Serial.printf("Destroying player %d\n", i);
            Players[i].Destroy();
        }
        Players.Clear();

        for (int i = 0; i < count; ++i)
        {
            Serial.printf("Loading player %d:\n", i);

            uint32_t color = 0;
            readUInt(eeAddress, color);
            Serial.printf("Color=%d", color);

            byte pin = 0;
            readByte(eeAddress, pin);
            Serial.printf(" Pin=%d", pin);

            char name[MAX_NAME_LENGTH];
            readString(eeAddress, (byte*)name);
            Serial.printf(" Name=%s\n", name);

            Players.Add(Player(color, pin, name));
            Serial.println("Player loaded!");
        }

        readWord(eeAddress, count);
        Serial.printf("%d obstacle count\n", count);

        Obstacles.Clear();

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Loading obstacle %d:\n", i);

            uint32_t color = 0;
            readUInt(eeAddress, color);
            Serial.printf("Color=%d", color);

            word start = 0;
            readWord(eeAddress, start);
            Serial.printf(" Start=%d", start);

            word end = 0;
            readWord(eeAddress, end);
            Serial.printf(" End=%d", end);

            word type;
            readWord(eeAddress, type);
            Serial.printf(" Type=%d", type);

            switch ((IObstacle::ObstacleType)type)
            {
            case IObstacle::OBSTACLE_OIL:
            {
                word pressDelay = 0;
                readWord(eeAddress, pressDelay);
                Serial.printf(" Delay=%d\n", pressDelay);

                OilObstacle *oil = new OilObstacle(start, end, color);
                Serial.println("Oil obstacle created");
                oil->setPressDelay(pressDelay);

                Obstacles.Add(oil);
                Serial.println("Oil obstacle loaded!");
            }
            break;

            case IObstacle::OBSTACLE_RAMP:
            {
                byte height = 0;
                readByte(eeAddress, height);
                Serial.printf(" height=%d", height);

                word style;
                readWord(eeAddress, style);
                Serial.printf(" style=%d\n", style);

                RampObstacle *ramp = new RampObstacle(start, end, height, color, (RampObstacle::RampStyle)style);
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