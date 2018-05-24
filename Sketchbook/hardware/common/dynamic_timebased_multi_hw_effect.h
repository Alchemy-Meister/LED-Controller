#ifndef DYNAMIC_TIMEBASED_MULTI_HW_EFFECT
#define DYNAMIC_TIMEBASED_MULTI_HW_EFFECT

#include <stdint.h>
#include "color/float_color.h"
#include "color/color_speed.h"
#include "timebased_multi_hw_effect.h"

class DynamicTimeBasedMultiHWEffect : public TimeBasedMultiHWEffect
{
protected:
    ColorSpeed colorSpeed;

    float componentTransition(const float component,
            const uint8_t targetComp, const float deltaTime,
            FloatColor::RGB compIdentifier);

    uint16_t getComponentSpeed(FloatColor::RGB) const;
public:
    DynamicTimeBasedMultiHWEffect(const LPD8806 strip);

    void transition(FloatColor &color, const Color &target,
        const float deltaTime);

    virtual void setSpeed(const float speed) = 0;
};

#endif
