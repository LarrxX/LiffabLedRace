#include "RaceConfig.h"

#ifdef USE_SPIFFS
#include <SPIFFS.h>
File file;
static const char configFileName[] = "/olr.cfg";
#else
#include <EEPROM.h>
#endif

#include "Car.h"
#include "Controller.h"
#include "OilObstacle.h"
#include "RampObstacle.h"
#include "Coroutines/LedLightingCoroutine.h"

namespace RaceConfig
{
    bool RaceStarted = true;
    bool EasyMode = false;

    word MaxLoops = DEFAULT_LOOPS;
    word MaxLED = DEFAULT_LED;

    DynamicArray<Player> Players(MAX_PLAYERS);
    DynamicPointerArray<IObstacle *> Obstacles(2);

    char RecordName[MAX_NAME_LENGTH]="";
    unsigned long RecordTime = ULONG_MAX;

    Adafruit_NeoPixel track(MaxLED, PIN_LED, NEO_GRB + NEO_KHZ800);
    
#ifdef LED_CIRCLE
    Adafruit_NeoPixel circle(MAXLEDCIRCLE, PIN_CIRCLE, NEO_GRB + NEO_KHZ800);
#endif //LED_CIRCLE

    void writeWord(int &offset, word data)
    {
#ifdef USE_SPIFFS
        file.write((uint8_t*)&data, sizeof(word));
#else
        EEPROM.writeUInt(offset, data);
#endif
        offset += sizeof(int);
    }

    void readWord(int &offset, word &data)
    {
#ifdef USE_SPIFFS
        file.read((uint8_t*)&data, sizeof(word));
#else
        EEPROM.get(offset, data);
#endif
        offset += sizeof(word);
    }

    void writeUInt(int &offset, uint32_t data)
    {
        writeWord(offset, (word)data);
    }

    void readUInt(int &offset, uint32_t &data)
    {
        word wordData;
        readWord(offset, wordData);
        data = wordData;
    }

    void writeULong(int& offset, unsigned long data)
    {
#ifdef USE_SPIFFS
        file.write((uint8_t*)&data, sizeof(unsigned long));
#else
        EEPROM.writeULong(offset, data);
#endif
        offset += sizeof(int);
    }

    void readULong(int &offset, unsigned long &data)
    {
#ifdef USE_SPIFFS
        file.read((uint8_t*)&data, sizeof(unsigned long));
#else
        EEPROM.get(offset, data);
#endif
        offset += sizeof(word);
    }

    void writeByte(int &offset, byte data)
    {
#ifdef USE_SPIFFS
        file.write(data);
#else
        EEPROM.writeByte(offset, data);
#endif

        offset += sizeof(byte);
    }

    void readByte(int &offset, byte &data)
    {
#ifdef USE_SPIFFS
        file.readBytes((char *)&data, 1);
#else
        EEPROM.get(offset, data);
#endif
        offset += sizeof(byte);
    }

    void writeString(int &offset, const char *data)
    {
        size_t len = strlen(data) + 1;

#ifdef USE_SPIFFS
        writeWord(offset, len);
        file.write((uint8_t *)data, len);
#else
        EEPROM.writeBytes(offset, data, len);
#endif
        offset += len * sizeof(char);
    }

    void readString(int &offset, char *data)
    {
#ifdef USE_SPIFFS
        size_t len;
        readWord(offset,len);
        file.read((uint8_t *)data, len);
        offset += len * sizeof(char);
#else

        for (int i = 0; i < MAX_NAME_LENGTH; ++i)
        {
            readByte(offset, data[i]);
            if (data[i] == '\0')
            {
                return;
            }
        }
#endif
    }

    void SaveRecord(int &offset)
    {
#ifdef USE_SPIFSS
        file = SPIFFS.open(configFileName, "a");
        if (!file)
        {
            Serial.println("Error opening file for saving new record.");
            return;
        }
        file.seek(offset);
#endif
        //Always reserve the maximum allowed size for a name so we don't accidentally wipe the data after it when we update only this section
        int begin = offset;
        writeString(offset, RecordName);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        writeULong(offset, RecordTime);

#ifdef USE_SPIFSS
        file.close();
#endif
    }

    void LoadRecord(int& offset)
    {
        int begin = offset;
        readString(offset, RecordName);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        readULong(offset, RecordTime);
    }

    void deleteRecord()
    {
        RecordName[0] = '\0';
        RecordTime = ULONG_MAX;
        int offset = strlen(SAVE_FILE_VERSION)+1;
        SaveRecord(offset);
    }

    void Save()
    {
#ifdef USE_SPIFFS
        file = SPIFFS.open(configFileName, "w");
        if (!file)
        {
            Serial.println("Unable to open SPIFFS file for saving!");
            return;
        }
        file.seek(0);
#endif
        int eeAddress = 0;
        Serial.printf("Saving file version \"%s\".\n", SAVE_FILE_VERSION);
        writeString(eeAddress, SAVE_FILE_VERSION);
        
        Serial.println("Saving record.");
        SaveRecord(eeAddress);

        Serial.printf("Saving Max Loops %d.\n", MaxLoops);
        writeWord(eeAddress, MaxLoops);

        Serial.printf("Saving Max LED %d.\n", MaxLED);
        writeWord(eeAddress, MaxLED);

        size_t count = Players.Count();
        Serial.printf("Saving Player Count %d.\n", count);
        writeWord(eeAddress, count);

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Saving Player %d:\n", i);
            Player &player = Players[i];
            
            Serial.printf("\tColor %d\n", player.getColor());
            writeUInt(eeAddress, player.getColor());
            
            Serial.printf("\tPin %d\n", player.controller().getPin());
            writeByte(eeAddress, player.controller().getPin());
            
            Serial.printf("\tLighting Pin %d\n", player.getLightingPin());
            writeWord(eeAddress, player.getLightingPin());

            Serial.printf("\tName \"%s\"\n", player.getName());
            writeString(eeAddress, player.getName());
        }
        
        count = Obstacles.Count();
        Serial.printf("Saving Obstacle Count %d.\n", count);
        writeWord(eeAddress, count);

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Saving Obstacle %d:\n", i);
            IObstacle *obstacle = Obstacles[i];

            Serial.printf("\tColor %d\n", obstacle->getColor());
            writeUInt(eeAddress, obstacle->getColor());

            Serial.printf("\tStart %d\n", obstacle->getStart());
            writeWord(eeAddress, obstacle->getStart());

            Serial.printf("\tEnd %d\n", obstacle->getEnd());
            writeWord(eeAddress, obstacle->getEnd());

            Serial.printf("\tType %d\n", obstacle->getType());
            writeWord(eeAddress, obstacle->getType());

            switch (obstacle->getType())
            {
            case IObstacle::OBSTACLE_OIL:
            {
                OilObstacle *oil = static_cast<OilObstacle *>(obstacle);
                
                Serial.printf("\tOil Press Delay %d\n", oil->getPressDelay());
                writeWord(eeAddress, oil->getPressDelay());
            }
            break;

            case IObstacle::OBSTACLE_RAMP:
            {
                RampObstacle *ramp = static_cast<RampObstacle *>(obstacle);

                Serial.printf("\tRamp Height %d\n", ramp->getHeight());
                writeByte(eeAddress, ramp->getHeight());

                Serial.printf("\tRamp Style %d\n", ramp->getStyle());
                writeWord(eeAddress, ramp->getStyle());
            }
            break;
            }
        }
#if defined(ESP32) && !defined(USE_SPIFFS)
        EEPROM.commit();
#endif

#ifdef USE_SPIFFS
        file.close();
#endif
        Serial.println("Saving done!");
    }

    bool Load()
    {
#ifdef USE_SPIFFS
        file = SPIFFS.open(configFileName, "r");

        if (!file)
        {
            Serial.println("Unable to open SPIFFS file for loading!");
            return false;
        }
        file.seek(0);
#endif
        Serial.println("Loading...");
        int eeAddress = 0;

        char version[MAX_NAME_LENGTH];
        readString(eeAddress, version);
        Serial.printf("Saved file is version \"%s\"; current supported version is \"%s\".\n", version, SAVE_FILE_VERSION);
        if (strcmp(version, SAVE_FILE_VERSION) != 0)
        {
            Serial.println("Wrong save file version.");
#ifdef USE_SPIFFS
            file.close();
#endif
            return false;
        }

        LoadRecord(eeAddress);
        
        readWord(eeAddress, MaxLoops);
        Serial.printf("Loaded Maxloops=%d\n", MaxLoops);

        readWord(eeAddress, MaxLED);
        Serial.printf("Loaded Maxled=%d\n", MaxLED);
        track.updateLength(MaxLED);
        Serial.println("Track length updated");

        word count;
        readWord(eeAddress, count);
        Serial.printf("Player Count %d\n", count);

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
            Serial.printf("Loading Player %d:\n", i);

            uint32_t color = 0;
            readUInt(eeAddress, color);
            Serial.printf("\tColor %d\n", color);

            byte pin = 0;
            readByte(eeAddress, pin);
            Serial.printf("\tPin %d\n", pin);

            word lightPin = 0;
            readWord(eeAddress, lightPin);
            Serial.printf("\tLight Pin %d\n", lightPin);

            char name[MAX_NAME_LENGTH];
            readString(eeAddress, name);
            Serial.printf("\tName \"%s\"\n", name);

            Players.Add(Player(color, pin, lightPin, name));
            Serial.println("Player loaded");
        }

        readWord(eeAddress, count);
        Serial.printf("Obstacle Count %d.\n", count);

        Obstacles.Clear();

        for (size_t i = 0; i < count; ++i)
        {
            Serial.printf("Loading Obstacle %d:\n", i);

            uint32_t color = 0;
            readUInt(eeAddress, color);
            Serial.printf("\tColor %d\n", color);

            word start = 0;
            readWord(eeAddress, start);
            Serial.printf("\tStart %d\n", start);

            word end = 0;
            readWord(eeAddress, end);
            Serial.printf("\tEnd %d\n", end);

            word type;
            readWord(eeAddress, type);
            Serial.printf("\tType %d\n", type);

            switch ((IObstacle::ObstacleType)type)
            {
            case IObstacle::OBSTACLE_OIL:
            {
                word pressDelay = 0;
                readWord(eeAddress, pressDelay);
                Serial.printf("\tOil Press Delay %d\n", pressDelay);

                OilObstacle *oil = new OilObstacle(start, end, color);
                Serial.println("Oil obstacle created");
                oil->setPressDelay(pressDelay);

                Obstacles.Add(oil);
                Serial.println("Oil obstacle loaded");
            }
            break;

            case IObstacle::OBSTACLE_RAMP:
            {
                byte height = 0;
                readByte(eeAddress, height);
                Serial.printf("\tRamp Height %d\n", height);

                word style;
                readWord(eeAddress, style);
                Serial.printf("\tRamp Style %d\n", style);

                RampObstacle *ramp = new RampObstacle(start, end, height, color, (RampObstacle::RampStyle)style);
                Serial.println("Ramp obstacle created");

                Obstacles.Add(ramp);
                Serial.println("Ramp obstacle loaded");
            }
            break;
            }
        }
        Serial.println("Loading done!");
#ifdef USE_SPIFFS
        file.close();
#endif
        return true;
    }

    bool checkAndSaveRecord(const char* name, unsigned long time)
    {
        if( time > RecordTime )
        {
            return false;
        }
        strcpy(RecordName, name);
        RecordTime = time;
        int offset = strlen(SAVE_FILE_VERSION) + 1;

        SaveRecord(offset);

        return true;
    }

    void Delete()
    {
#ifdef USE_SPIFFS
        SPIFFS.remove(configFileName);
#endif
    }
};