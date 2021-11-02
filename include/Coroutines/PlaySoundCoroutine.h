#pragma once

#include "Coroutines/IBlockingCoroutine.h"

class PlaySoundCoroutine : public IBlockingCoroutine
{
    protected:
        unsigned int _frequency;
        uint32_t _delay;
    
    public:
        void setParameters( unsigned int frequency, uint32_t delay);
        int runCoroutine() override;
};