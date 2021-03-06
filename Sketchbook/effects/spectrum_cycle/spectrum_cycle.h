#ifndef SPECTRUM_CYCLE_H
#define SPECTRUM_CYCLE_H

#include <stdint.h>
#include <math.h>
#include "hardware/common/dynamic_timebased_multi_hw_effect.h"

/* SPECTRUM CYCLING EFFECT
 * ------------------------------
 * This effect loops a RGB color bi-dimensional array, makes a color
 * transition and then stays in the target color for the same amount of time.
 */
class SpectrumCycle : public DynamicTimeBasedMultiHWEffect {
private:
    // Duration for single color transition.
    static const uint32_t transitionDuration;

    // Declares bi-dimensional array for storing spectrum's RGB colors.
    static const uint8_t colorSize = 16;
    static const Color spectrumColors[colorSize];

    uint32_t currentTransitionDuration;

    // Index for accessing RGB color bi-dimensional array.
    int8_t colorIndex;

    // Boolean to check if the effect needs initialization.
    uint8_t spectrumEffectInitialization;

    Color target;

    void calculateSpeed(const FloatColor currentColor,
        const Color targetColor);

    uint16_t calculateCompSpeed(const float currentComp,
        const uint8_t targetComp);
public:
    SpectrumCycle(const LPD8806 strip);

    void initializeEffect();

    void setSpeed(const float speed);

    void processEffect(FloatColor &color, const float deltaTime);

    void updateColorIndex();
};

#endif
