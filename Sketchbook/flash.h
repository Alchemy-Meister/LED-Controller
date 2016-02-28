#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>
#include "color.h"
#include "float_color.h"
#include "timebased_effect.h"
#include "Arduino.h"

class Flash : public TimeBasedEffect
{
	private:
		// Duration of flashing and and off parts.
		static const uint32_t offDuration;
		static const uint32_t middleOffDuration;
		static const uint32_t flashDuration;

		// Boolean to check if the effect is flashing.
		uint8_t flashing;

		// Boolean to check if the effects needs to doubleFash.
		uint8_t doubleFlash;

		// Boolean to check if the effect is in middle off.
		uint8_t middleOff;

		uint8_t firstFlash;

		Color flashColor;
	public:
		Flash();

		void initializeEffect(const Color &color, const uint8_t doubleFash);
		void setStartTime(const uint32_t currentTime);
		void processEffect(FloatColor &color);
	
};

#endif