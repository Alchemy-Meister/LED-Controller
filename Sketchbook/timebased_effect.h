#ifndef TIMEBASED_EFFECT
#define TIMEBASED_EFFECT

#include <stdint.h>
#include "color.h"
#include "float_color.h"

class TimeBasedEffect
{
	protected:
		// Time when effect starts in milliseconds.
		uint32_t startTime;
		// Passed time since effect started in milliseconds.
		uint32_t elapsedTime;
};

#endif