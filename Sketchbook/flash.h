#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>
#include "float_color.h"
#include "Arduino.h"

class Flash
{
	private:
		static const uint32_t offDuration;
		static const uint32_t fashDuration;
	public:
		Flash();

		void initializeEffect();

		void processEffect(FloatColor &color, const float deltaTime);
	
};

#endif