#include "TimeUtils.h"

namespace TimeUtils
{
    String getTimeString(unsigned long time)
    {
        return String((time/60000)) + ":"+ ((time % 60000) / 1000.f);
    }
};