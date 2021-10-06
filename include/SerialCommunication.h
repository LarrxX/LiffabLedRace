#pragma once

#include "Defines.h"

class Stream;

class SerialCommunication
{
private:
    static SerialCommunication* _instance;
    SerialCommunication();

protected:
    char _receiveBuffer[REC_COMMAND_BUFLEN];
    char _transmitBuffer[TX_COMMAND_BUFLEN];

    Stream *_stream;
    int _receiveIndex;

public:
    static SerialCommunication& instance();

    void SendCommand(const char *formattedString, ...);
    const char* ReadSerial(int& length);

protected:
    void WriteToStream(const char *command);
};