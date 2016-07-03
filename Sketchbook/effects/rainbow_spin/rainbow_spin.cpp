#include "rainbow_spin.h"

const uint32_t RainbowSpin::transitionDuration = 7000L;

const int16_t RainbowSpin::wheelRange = Color::SIGNED_RANGE * 3;

RainbowSpin::RainbowSpin(const LPD8806 strip) : TimeBasedLPD8806Effect(strip) {
	this->currentTransitionDuration = RainbowSpin::transitionDuration
		* strip.numPixels() / defaultStripLength;
	this->currentRedPosition = 0;
	this->clockwise = 1;
}

void RainbowSpin::initializeEffect() {
	this->startTime = micros();
	for(uint16_t i = 0; i < this->strip.numPixels(); i++) {
		this->strip.setPixelColor(
			i,
			Color::colorFromWheel(
				(i * this->wheelRange / this->strip.numPixels())
				% wheelRange, Color::SIGNED_RANGE)
		);
	}
	this->strip.show();
}

void RainbowSpin::processEffect() {
	// Updates cycling elapsed time.
	this->elapsedTime = micros() - this->startTime;
	if(this->elapsedTime >= this->currentTransitionDuration) {

		if(clockwise) {
			this->currentRedPosition -= elapsedTime
				/ this->currentTransitionDuration
				* this->wheelRange / this->strip.numPixels();
		} else {
			this->currentRedPosition += elapsedTime
				/ this->currentTransitionDuration
				* this->wheelRange / this->strip.numPixels();
		}

		if(this->currentRedPosition > this->wheelRange)
		{
			this->currentRedPosition = int16_t(this->currentRedPosition)
				% this->wheelRange;
		} else if(this->currentRedPosition <= this->wheelRange * -1) {
			this->currentRedPosition = this->wheelRange
			- this->currentRedPosition * -1;
		}

		for(uint16_t i = 0; i < this->strip.numPixels(); i++) {
			float colorIndex = float(i * this->wheelRange) / this->strip.numPixels()
			+ this->currentRedPosition;

			if(colorIndex < 0) {
				colorIndex = this->wheelRange + colorIndex;
			}

			this->strip.setPixelColor(
				i,
				Color::colorFromWheel(
					int16_t(colorIndex) % this->wheelRange,
					Color::SIGNED_RANGE
				)
			);
		}
		this->strip.show();
		this->startTime = micros();
	}
}

void RainbowSpin::setSpeed(const float speed) {
	this->currentTransitionDuration = RainbowSpin::transitionDuration
		* strip.numPixels() / this->defaultStripLength / speed;
}

void RainbowSpin::setSense(const uint8_t clockwise) {
	this->clockwise = clockwise;
}
