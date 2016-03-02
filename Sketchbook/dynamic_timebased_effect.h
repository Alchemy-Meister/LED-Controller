#ifndef DYNAMIC_TIMEBASED_EFFECT
#define DYNAMIC_TIMEBASED_EFFECT

#include <stdint.h>
#include "color_speed.h"
#include "timebased_effect.h"

class DynamicTimeBasedEffect : public TimeBasedEffect
{
	protected:
		ColorSpeed colorSpeed;

		float componentTransition(const float component,
				const uint8_t targetComp, const float deltaTime,
				FloatColor::RGB compIdentifier);

		uint16_t getComponentSpeed(FloatColor::RGB) const;
	public:

		void transition(FloatColor &color, const Color &target,
			const float deltaTime);
		
};

#endif