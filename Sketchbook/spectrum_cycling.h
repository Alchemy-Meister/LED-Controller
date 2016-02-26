#ifndef SPECTRUM_CYCLING_H
#define SPECTRUM_CYCLING_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "color.h"
#include "float_color.h"
#include "timebased_effect.h"
#include "Arduino.h"

/* SPECTRUM CYCLING EFFECT
 * ------------------------------
 * This effect loops a RGB color bi-dimensional array, makes a color 
 * transition and then stays in the target color for the same amount of time.
 */
class SpectrumCycling:public TimeBasedEffect {
	private:
		// Index for accessing RGB color bi-dimensional array.
		int8_t colorIndex;

		// Boolean to check if the effect needs initialization.
		uint8_t spectrumEffectInitialization;

		void calculateSpeed(const FloatColor currentColor,
			const Color targetColor);

		float calculateCompSpeed(const float currentComp,
			const uint8_t targetComp);
	public:
		// Duration for single color transition.
		static const float transitionDuration;
		static const float transitionDurationSec;

		// Declares bi-dimensional array for storing spectrum's RGB colors.
		static const uint8_t colorSize = 16;
		static const Color spectrumColors[colorSize];

		SpectrumCycling();

		void initializeEffect();

		uint8_t needsInitialization();
		void setInitializationFlag(const uint8_t initialize);
		
		void processEffect(FloatColor &color, const float deltaTime);

		void updateColorIndex();
};

#endif