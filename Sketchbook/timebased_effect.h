#ifndef TIMEBASED_EFFECT
#define TIMEBASED_EFFECT

#include <stdint.h>
#include "color.h"
#include "float_color.h"
#include "Arduino.h"
#include "LPD8806.h"
#include "SPI.h"

class TimeBasedEffect
{
	protected:
		// Maximum brightness for multiple HW.
		static const uint8_t LPD8806_MAX = 127;
		static const uint8_t MULTI_MAX = 255;

		// Time when effect starts in milliseconds.
		uint32_t startTime;
		// Passed time since effect started in milliseconds.
		uint32_t elapsedTime;

		// Boolean to check if LPD8806 is being used.
		uint8_t singleLEDController;

		// LPD8806 micro-controller LED strip.
		LPD8806 strip;

	public:
		TimeBasedEffect();

		void enableSingleLEDController(const uint8_t enable);
		uint8_t isSingleLEDControllerEnabled() const;

		void setStrip(const LPD8806 strip);
		LPD8806 getStrip() const;

		void updateStripColor(
			const Color currentColor,
			const uint8_t redPin,
			const uint8_t greenPin,
			const uint8_t bluePin);
};

#endif