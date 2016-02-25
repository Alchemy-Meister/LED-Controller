#include "timebased_effect.h"

void TimeBasedEffect::setStartTime(uint32_t currentTime) {
	this->startTime = currentTime;
}

float TimeBasedEffect::getComponentSpeed(FloatColor::RGB component) const {
	switch(component) {
		case FloatColor::RED:
			return this->colorSpeed.getRed();
		case FloatColor::GREEN:
			return this->colorSpeed.getGreen();
		case FloatColor::BLUE:
			return this->colorSpeed.getBlue();
	}
}


// This function calculates the next color transition, depending on the 
// target color, and effect speed.
FloatColor TimeBasedEffect::transition(const FloatColor &color,
	const Color &target, const float deltaTime)
{
	FloatColor transitedColor = FloatColor();
	transitedColor.setRed(this->componentTransition(
		color.getRed(), target.getRed(), deltaTime, FloatColor::RED));
	transitedColor.setGreen(this->componentTransition(
		color.getGreen(), target.getGreen(), deltaTime, FloatColor::GREEN));
	transitedColor.setBlue(this->componentTransition(
		color.getBlue(), target.getBlue(), deltaTime, FloatColor::BLUE));
	return transitedColor;
}

float TimeBasedEffect::componentTransition(const float component,
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
		return FloatColor::colorUpLimiter(component - 
			this->getComponentSpeed(compIdentifier) * deltaTime, targetComp);
	}	
 }