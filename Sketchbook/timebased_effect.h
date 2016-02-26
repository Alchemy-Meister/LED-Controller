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

		FloatColor colorSpeed;

		float componentTransition(const float component,
			const uint8_t targetComp, const float deltaTime,
			FloatColor::RGB compIdentifier);

		float getComponentSpeed(FloatColor::RGB) const;
	public:
		void setStartTime(uint32_t currentTime);

		void transition(FloatColor &color, const Color &target,
			const float deltaTime);
};

#endif