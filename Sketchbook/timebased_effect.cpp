#include "timebased_effect.h"

TimeBasedEffect::TimeBasedEffect() {}

void TimeBasedEffect::enableSingleLEDController(const uint8_t enable) {
	this->singleLEDController = enable;
}

uint8_t TimeBasedEffect::isSingleLEDControllerEnabled() const {
	return this->singleLEDController;
}

void TimeBasedEffect::setStrip(const LPD8806 strip) {
	this->strip = strip;
}

LPD8806 TimeBasedEffect::getStrip() const {
	return this->strip;
}

void TimeBasedEffect::updateStripColor(const Color currentColor,
	const uint8_t redPin, const uint8_t greenPin, const uint8_t bluePin) 
{
	if(this->singleLEDController) {
		for(int i = 0; i < strip.numPixels(); i++) {
			strip.setPixelColor(
				i,
				currentColor.getRed(),
				currentColor.getGreen(),
				currentColor.getBlue()
			);
		}
		strip.show();
	} else {
		analogWrite(redPin, currentColor.getRed());
		analogWrite(greenPin, currentColor.getGreen());
		analogWrite(bluePin, currentColor.getBlue());
	}
}