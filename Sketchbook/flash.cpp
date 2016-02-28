#include "flash.h"

const uint32_t Flash::offDuration = 450000;
const uint32_t Flash::middleOffDuration = 100000;
const uint32_t Flash::flashDuration = 100000;

Flash::Flash() {}

void Flash::initializeEffect(const Color &color, const uint8_t doubleFlash) {
	this->flashing = 0;
	this->firstFlash = 1;
	this->flashColor = color;

	if(doubleFlash)
		this->doubleFlash = 1;
	else
		this->doubleFlash = 0;
}

void Flash::setStartTime(const uint32_t currentTime) {
	this->startTime = currentTime;
}

void Flash::processEffect(FloatColor &color) {
	this->elapsedTime = micros() - this->startTime;

	if(this->middleOff) {
		color = FloatColor(0, 0, 0);

		if(this->elapsedTime >= Flash::middleOffDuration) {
			this->flashing = 1;
			this->middleOff = 0;
			this->firstFlash = 0;
			this->startTime = micros();
		}
	} else {
		if(this->flashing) {
			color = FloatColor(this->flashColor);
			
			if(this->elapsedTime >= Flash::flashDuration) {
				if(this->doubleFlash && this->firstFlash) {
					this->middleOff = 1;
				} else {
					this->flashing = 0;
				}
				this->startTime = micros();
			}
		} else {
			color = FloatColor(0, 0, 0);

			if(this->elapsedTime >= Flash::offDuration) {
				this->flashing = 1;
				this->firstFlash = 1;
				this->startTime = micros();
			}
		}
	}
}
