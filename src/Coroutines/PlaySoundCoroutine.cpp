#include "Coroutines/PlaySoundCoroutine.h"

#include "Defines.h"

#ifdef ESP32
#include <ESP32Tone.h>
#endif

void PlaySoundCoroutine::setParameters( unsigned int frequency, uint32_t delay)
{
    reset();
    noTone(PIN_AUDIO);
    _frequency = frequency;
    _delay = delay;
}

int PlaySoundCoroutine::runCoroutine()
{
    COROUTINE_BEGIN();
    
    tone( PIN_AUDIO, _frequency);
    BLOCKING_COROUTINE_DELAY(_delay);
    noTone(PIN_AUDIO);
    
    COROUTINE_END();
}