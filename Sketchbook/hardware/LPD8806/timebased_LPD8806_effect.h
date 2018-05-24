#ifndef TIME_BASED_LPD8806_EFFECT_H
#define TIME_BASED_LPD8806_EFFECT_H

#include "LPD8806_effect.h"

class TimeBasedLPD8806Effect : public LPD8806Effect
{
protected:
    // Time when effect starts in milliseconds.
    uint32_t startTime;
    // Passed time since effect started in milliseconds.
    uint32_t elapsedTime;

public:
    TimeBasedLPD8806Effect(const LPD8806 strip);

    virtual void setSpeed(const float speed) = 0;
};

#endif
