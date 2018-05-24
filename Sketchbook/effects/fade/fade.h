#ifndef FADE_H
#define FADE_H

#include <math.h>
#include "hardware/common/dynamic_timebased_multi_hw_effect.h"

/* FADE AND BREATHING EFFECTS.
 * ------------------------------
 * This effect fades in and out for a single color for a specific amount of
 * time. If the breathing variable is TRUE, after the fade out it stays
 * off for a determined amount of time and then repeats. If the breathing
 * variable is FALSE this last part is omitted.
 */
class Fade : public DynamicTimeBasedMultiHWEffect {
private:
    // Duration of fade ins and outs.
    static const uint32_t fadeDuration;
    // Duration of the off time for the breathing part effect.
    static const uint32_t offDuration;

    // Float scale factor for effect speed.
    float fadeSpeed;
    float breathSpeed;

    // Actual duration of fade ins and outs.
    uint32_t currentFadeDuration;
    uint32_t currentOffDuration;

    // Boolean to check if the effect is fade in/out.
    uint8_t fadeIn;
    // Boolean to check if the effects needs to breath.
    uint8_t breath;
    // Boolean to check if the effect is breathing.
    uint8_t breathing;

    FloatColor baseColor;

    void calculateSpeed(const FloatColor &color);
public:
    Fade(const LPD8806 strip);

    void initializeEffect();
    void initializeEffect(const FloatColor &color, const uint8_t breath);
    void setStartTime(const uint32_t currentTime);
    void setSpeed(const float speed);
    void processEffect(FloatColor &color, const float deltaTime);
};

#endif
