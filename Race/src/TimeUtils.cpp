#include "TimeUtils.h"
#include "RaceConfig.h"

namespace TimeUtils
{
    String getTimeString(unsigned long time)
    {
        return String((time/60000)) + ":"+ ((time % 60000) / 1000.f);
    }

    void resetRecord(RaceConfig::Record* record)
    {
        record->_name[0] = '\0';
        record->_time = 0;
        record->_color = 0;
        record->_date = 0;
    }
};