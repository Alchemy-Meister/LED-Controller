#ifndef FADE_H
#define FADE_H

#include <math.h>
#include "timebased_effect.h"
#include "float_color.h"
#include "Arduino.h"

/* FADE AND BREATHING EFFECTS.
 * ------------------------------
 * This effect fades in and out for a single color for a specific amount of
 * time. If the breathing variable is TRUE, after the fade out it stays
 * off for a determined amount of time and then repeats. If the breathing
 * variable is FALSE this las part is omitted.
 */
class Fade : public TimeBasedEffect {
	private:
		// Boolean to check if the effect is fade in/out.
		uint8_t fadeIn;
		// Boolean to check if the effects needs to breath.
		uint8_t breath;
		// Boolean to check if the effect is breathing.
		uint8_t breathing;

		void calculateSpeed(const FloatColor &color);
	public:
		// Duration of fade ins and outs.
		static const float fadeDuration;
		static const float fadeDurationSeconds;

		// Duration of the off time for the breathing part effect.
		static const float offDuration;

		Fade();

		void setFadeStartTime(uint32_t currentTime);

		void initializeEffect(const FloatColor &color, const uint8_t breath);
		void processEffect(FloatColor &color, const float deltaTime);
};

#endif