#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>
#include "color/float_color.h"
#include "hardware/common/timebased_multi_hw_effect.h"

class Flash : public TimeBasedMultiHWEffect
{
private:
    // Duration of flashing and and off parts.
    static const uint32_t offDuration;
    static const uint32_t middleOffDuration;
    static const uint32_t flashDuration;

    // Float scale factor for effect speed.
    float flashSpeed;
    float doubleFlashSpeed;

    // Actual duration of flashes.
    uint32_t currentOffDuration;
    uint32_t currentMiddleOffDuration;
    uint32_t currentFlashDuration;

    // Boolean to check if the effect is flashing.
    uint8_t flashing;

    // Boolean to check if the effects needs to doubleFash.
    uint8_t doubleFlash;

    // Boolean to check if the effect is in middle off.
    uint8_t middleOff;

    uint8_t firstFlash;

    Color flashColor;
public:
    Flash(const LPD8806 strip);

    void initializeEffect();
    void initializeEffect(const Color &color, const uint8_t doubleFash);
    void setStartTime(const uint32_t currentTime);
    void setSpeed(const float speed);
    void processEffect(FloatColor &color);

};

#endif
