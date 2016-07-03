#ifndef RAINBOW_SPIN_H
#define RAINBOW_SPIN_H

#include <stdint.h>
#include <math.h>
#include "hardware/LPD8806/timebased_LPD8806_effect.h"

/* RAINBOW SPIN EFFECT [WORK ONLY FOR LDP8806]
 * ------------------------------
 * This effect loops a RGB color bi-dimensional array, makes a color
 * transition and then stays in the target color for the same amount of time.
 */
class RainbowSpin : public TimeBasedLPD8806Effect {

	private:
		static const uint16_t defaultStripLength = 32;

		static const int16_t wheelRange;

		// LED transition duration for the default led length strip.
		static const uint32_t transitionDuration;

		// LED transition duration for current strip length.
		uint32_t currentTransitionDuration;

		// Position of initial Red position resgarding color wheel
		float currentRedPosition;

		// Boolean to check sense of the spin.
		uint8_t clockwise;

	public:
		RainbowSpin(const LPD8806 strip);

		void initializeEffect();
		void processEffect();

		void setSpeed(const float speed);
		void setSense(const uint8_t clockwise);
};

#endif
