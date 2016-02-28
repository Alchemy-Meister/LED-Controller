#ifndef SPECTRUM_CYCLE_H
#define SPECTRUM_CYCLE_H

#include <stdint.h>
#include <math.h>
#include "color.h"
#include "float_color.h"
#include "dynamic_timebased_effect.h"
#include "Arduino.h"

/* SPECTRUM CYCLING EFFECT
 * ------------------------------
 * This effect loops a RGB color bi-dimensional array, makes a color 
 * transition and then stays in the target color for the same amount of time.
 */
class SpectrumCycle : public DynamicTimeBasedEffect {
	private:
		// Duration for single color transition.
		static const uint32_t transitionDuration;
		static const uint8_t transitionDurationSec;

		// Declares bi-dimensional array for storing spectrum's RGB colors.
		static const uint8_t colorSize = 16;
		static const Color spectrumColors[colorSize];

		// Index for accessing RGB color bi-dimensional array.
		int8_t colorIndex;

		// Boolean to check if the effect needs initialization.
		uint8_t spectrumEffectInitialization;

		Color target;

		void calculateSpeed(const FloatColor currentColor,
			const Color targetColor);

		uint8_t calculateCompSpeed(const float currentComp,
			const uint8_t targetComp);
	public:
		SpectrumCycle();

		void initializeEffect();
		
		void processEffect(FloatColor &color, const float deltaTime);

		void updateColorIndex();
};

#endif