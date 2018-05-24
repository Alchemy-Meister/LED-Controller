#ifndef MULTI_HW_EFFECT_H
#define MULTI_HW_EFFECT_H

#include "hardware/led5050strip/rgb_led_effect.h"
#include "hardware/LPD8806/LPD8806_effect.h"
#include "lib/LPD8806.h"

class MultiHWEffect : public RGBLedStrip, public LPD8806Effect
{
protected:
    // Boolean to check if LPD8806 is being used.
    uint8_t multiLEDController;

public:
    MultiHWEffect(LPD8806 strip);
    MultiHWEffect(LPD8806 strip, uint8_t isSingleControllerEnabled);

    void enableSingleLEDController(const uint8_t enable);
    uint8_t isSingleLEDControllerEnabled() const;

    void updateStripColor(const Color currentColor, const uint8_t redPin,
        const uint8_t greenPin, const uint8_t bluePin);
};

#endif
