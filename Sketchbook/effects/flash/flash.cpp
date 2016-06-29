#include "flash.h"

const uint32_t Flash::offDuration = 450000;
const uint32_t Flash::middleOffDuration = 100000;
const uint32_t Flash::flashDuration = 100000;

Flash::Flash(const LPD8806 strip) : TimeBasedMultiHWEffect(strip) {
	this->flashSpeed = 1;
	this->doubleFlashSpeed = 1;
	this->currentOffDuration = Flash::offDuration;
	this->currentMiddleOffDuration = Flash::middleOffDuration;
	this->currentFlashDuration = Flash::flashDuration;
}

void Flash::initializeEffect() {
	this->flashing = 0;
	this->firstFlash = 1;
}

void Flash::initializeEffect(const Color &color, const uint8_t doubleFlash) {
	Flash::initializeEffect();
	
	this->flashColor = color;

	if(doubleFlash) {
		this->doubleFlash = 1;
		this->currentOffDuration = Flash::offDuration / this->doubleFlashSpeed;
		this->currentMiddleOffDuration = Flash::middleOffDuration /
			this->doubleFlashSpeed;
		this->currentFlashDuration = Flash::flashDuration /
			this->doubleFlashSpeed;
	} else {
		this->doubleFlash = 0;
		this->currentOffDuration = Flash::offDuration / this->flashSpeed;
		this->currentMiddleOffDuration = Flash::middleOffDuration /
			this->flashSpeed;
		this->currentFlashDuration = Flash::flashDuration / this->flashSpeed;
	}
}

void Flash::setStartTime(const uint32_t currentTime) {
	this->startTime = currentTime;
}

void Flash::setSpeed(const float speed) {
	if(this->doubleFlash) {
		this->doubleFlashSpeed = speed;
		this->currentOffDuration = Flash::offDuration / this->doubleFlashSpeed;
		this->currentMiddleOffDuration = Flash::middleOffDuration /
			this->doubleFlashSpeed;
		this->currentFlashDuration = Flash::flashDuration /
			this->doubleFlashSpeed;
	} else {
		this->flashSpeed = speed;
		this->currentOffDuration = Flash::offDuration / this->flashSpeed;
		this->currentMiddleOffDuration = Flash::middleOffDuration /
			this->flashSpeed;
		this->currentFlashDuration = Flash::flashDuration / this->flashSpeed;
	}

}

void Flash::processEffect(FloatColor &color) {
	this->elapsedTime = micros() - this->startTime;

	if(this->middleOff) {
		color = FloatColor(0, 0, 0);

		if(this->elapsedTime >= this->currentMiddleOffDuration) {
			this->flashing = 1;
			this->middleOff = 0;
			this->firstFlash = 0;
			this->startTime = micros();
		}
	} else {
		if(this->flashing) {
			color = FloatColor(this->flashColor);

			if(this->elapsedTime >= this->currentFlashDuration) {
				if(this->doubleFlash && this->firstFlash) {
					this->middleOff = 1;
				} else {
					this->flashing = 0;
				}
				this->startTime = micros();
			}
		} else {
			color = FloatColor(0, 0, 0);

			if(this->elapsedTime >= this->currentOffDuration) {
				this->flashing = 1;
				this->firstFlash = 1;
				this->startTime = micros();
			}
		}
	}
}
