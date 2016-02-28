#include "flash.h"

const uint32_t Flash::offDuration = 400000;
const uint32_t Flash::flashDuration = 100000;

Flash::Flash() {}

void Flash::initializeEffect(const Color &color) {
	this->flashing = 0;
	this->flashColor = color;
}

void Flash::setStartTime(const uint32_t currentTime) {
	this->startTime = currentTime;
}

void Flash::processEffect(FloatColor &color) {
	this->elapsedTime = micros() - this->startTime;

	if(this->flashing) {
		color = FloatColor(this->flashColor);
		
		if(this->elapsedTime >= Flash::flashDuration) {
			this->flashing = 0;
			this->startTime = micros();
		}
	} else {
		color = FloatColor(0, 0, 0);

		if(this->elapsedTime >= Flash::offDuration) {
			this->flashing = 1;
			this->startTime = micros();
		}
	}
}
