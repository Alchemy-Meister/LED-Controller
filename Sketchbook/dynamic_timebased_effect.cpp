#include "dynamic_timebased_effect.h"

uint16_t DynamicTimeBasedEffect::getComponentSpeed(FloatColor::RGB component) const {
	switch(component) {
		case FloatColor::RED:
			return this->colorSpeed.getRedSpeed();
		case FloatColor::GREEN:
			return this->colorSpeed.getGreenSpeed();
		case FloatColor::BLUE:
			return this->colorSpeed.getBlueSpeed();
	}
}


// This function calculates the next color transition, depending on the 
// target color, and effect speed.
void DynamicTimeBasedEffect::transition(FloatColor &color,
	const Color &target, const float deltaTime)
{
	color.setRed(this->componentTransition(
		color.getRed(), target.getRed(), deltaTime, FloatColor::RED));
	color.setGreen(this->componentTransition(
		color.getGreen(), target.getGreen(), deltaTime, FloatColor::GREEN));
	color.setBlue(this->componentTransition(
		color.getBlue(), target.getBlue(), deltaTime, FloatColor::BLUE));
}

float DynamicTimeBasedEffect::componentTransition(const float component,
	const uint8_t targetComp, const float deltaTime,
	FloatColor::RGB compIdentifier)
{
 	// IF color needs to increment to reach the target.
	if(FloatColor::componentIsIncrement(component, targetComp)) {
		// ADDS relative increment.
		// Make correction if target value gets surpassed.
		return FloatColor::colorUpLimiter(component + 
			this->getComponentSpeed(compIdentifier) * deltaTime, targetComp);
	} else {
		// SUBTRACTS relative increment. (decrement)
		// Make correction if target value gets surpassed.
		return FloatColor::colorDownLimiter(component - 
			this->getComponentSpeed(compIdentifier) * deltaTime, targetComp);
	}	
}