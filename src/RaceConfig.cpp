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

namespace RaceConfig
{
    bool RaceStarted = true;

    word MaxLoops = DEFAULT_LOOPS;
    word MaxLED = DEFAULT_LED;

    DynamicArray<Player> Players(MAX_PLAYERS);
    DynamicPointerArray<IObstacle *> Obstacles(2);

    char RecordName[MAX_NAME_LENGTH]="";
    unsigned long RecordTime = ULONG_MAX;

    Adafruit_NeoPixel track = Adafruit_NeoPixel(MaxLED, PIN_LED, NEO_GRB + NEO_KHZ800);

#ifdef LED_CIRCLE
    Adafruit_NeoPixel circle = Adafruit_NeoPixel(MAXLEDCIRCLE, PIN_CIRCLE, NEO_GRB + NEO_KHZ800);


    uint32_t Wheel(byte WheelPos) {
        if(WheelPos < 85) {
        return circle.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        } 
        else if(WheelPos < 170) {
        WheelPos -= 85;
        return circle.Color(255 - WheelPos * 3, 0, WheelPos * 3);
        } 
        else {
        WheelPos -= 170;
        return circle.Color(0, WheelPos * 3, 255 - WheelPos * 3);
        }
    }

    // Circle led - Fill the dots one after the other with a color
    void colorWipe(uint32_t c, uint8_t wait) {
        for(uint16_t i=0; i<circle.numPixels(); i++) {
            circle.setPixelColor(i, c);
            circle.show();
            delay(wait);
        }
    }

    // Circle led - rainbow
    void rainbow(uint8_t wait) {
        uint16_t i, j;
        for(j=0; j<256; j++) {
            for(i=0; i<circle.numPixels(); i++) {
                circle.setPixelColor(i, Wheel((i*1+j) & 255));
            }
        circle.show();
        delay(wait);
        }
    }

    // Circle led - Theatre-style crawling lights.
    void theaterChase(uint32_t c, uint8_t wait) {
        for (int j=0; j<10; j++) {  //do 10 cycles of chasing
            for (int q=0; q < 3; q++) {
                for (int i=0; i < circle.numPixels(); i=i+3) {
                    circle.setPixelColor(i+q, c);    //turn every third pixel on
                }
                circle.show();
                delay(wait);
                for (int i=0; i < circle.numPixels(); i=i+3) {
                    circle.setPixelColor(i+q, 0);        //turn every third pixel off
                }
            }
        }
    }
#endif //LED_CIRCLE

    void writeWord(int &offset, word data)
    {
#ifdef USE_SPIFFS
        file.write(data);
#else
        EEPROM.writeUInt(offset, data);
#endif
        offset += sizeof(int);
    }

    void readWord(int &offset, word &data)
    {
#ifdef USE_SPIFFS
        data = file.read();
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
        file.write(data);
#else
        EEPROM.writeULong(offset, data);
#endif
        offset += sizeof(int);
    }

    void readULong(int &offset, unsigned long &data)
    {
#ifdef USE_SPIFFS
        data = file.read();
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
        data = file.readBytes((char *)&data, 1);
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

    void readString(int &offset, byte *data)
    {
#ifdef USE_SPIFFS
        word len;
        readWord(offset,len);
        file.readBytes((char*)data, len);
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
        writeString(offset, (char *)RecordName);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        writeULong(offset, RecordTime);

#ifdef USE_SPIFSS
        file.close();
#endif
    }

    void LoadRecord(int& offset)
    {
        int begin = offset;
        readString(offset, (byte*)RecordName);
        offset = begin + (MAX_NAME_LENGTH * sizeof(char));
        readULong(offset, RecordTime);
    }

    void deleteRecord()
    {
        RecordName[0] = '\0';
        RecordTime = ULONG_MAX;
        int offset = 0;
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

        SaveRecord(eeAddress);

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

#ifdef USE_SPIFFS
        file.close();
#endif
    }

    void Load()
    {
#ifdef USE_SPIFFS
        file = SPIFFS.open(configFileName, "r");

        if (!file)
        {
            Serial.println("Unable to open SPIFFS file for loading!");
            return;
        }
        file.seek(0);
#endif
        Serial.println("Loading...");
        int eeAddress = 0;

        LoadRecord(eeAddress);
        
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
            readString(eeAddress, (byte *)name);
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
#ifdef USE_SPIFFS
        file.close();
#endif
    }

    bool checkAndSaveRecord(const char* name, unsigned long time)
    {
        if( time > RecordTime )
        {
            return false;
        }
        strcpy(RecordName, name);
        RecordTime = time;
        int offset = 0;

        SaveRecord(offset);

        return true;
    }
};