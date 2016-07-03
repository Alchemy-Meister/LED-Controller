#include "rainbow_spin.h"

const uint32_t RainbowSpin::transitionDuration = 7000L;

const uint16_t RainbowSpin::wheelRange = Color::SIGNED_RANGE * 3;

RainbowSpin::RainbowSpin(const LPD8806 strip) : TimeBasedLPD8806Effect(strip) {
	this->currentTransitionDuration = RainbowSpin::transitionDuration
		* strip.numPixels() / defaultStripLength;
	this->currentRedPosition = 0;
	this->inverse = 0;
}

void RainbowSpin::initializeEffect() {
	this->startTime = micros();
	for(uint16_t i = 0; i < this->strip.numPixels(); i++) {
		this->strip.setPixelColor(
			i,
			Color::colorFromWheel(
				(i * wheelRange / this->strip.numPixels())
				% wheelRange, Color::SIGNED_RANGE)
		);
	}
	this->strip.show();
}

void RainbowSpin::processEffect() {
	// Updates cycling elapsed time.
	this->elapsedTime = micros() - this->startTime;
	if(this->elapsedTime >= this->currentTransitionDuration) {

		this->currentRedPosition += elapsedTime
			/ this->currentTransitionDuration
			* wheelRange / this->strip.numPixels();

		if(this->currentRedPosition > wheelRange) {
			this->currentRedPosition = uint16_t(this->currentRedPosition)
				% wheelRange;
		}

		for(uint16_t i = 0; i < this->strip.numPixels(); i++) {
			if(inverse) {
				this->strip.setPixelColor(
					i,
					Color::colorFromWheel((wheelRange - 1) -
						((uint16_t(i * wheelRange / this->strip.numPixels())
						+ uint16_t(this->currentRedPosition)) % wheelRange),
						Color::SIGNED_RANGE
					)
				);
			} else {
				this->strip.setPixelColor(
					i,
					Color::colorFromWheel(
						(uint16_t(i * wheelRange / this->strip.numPixels())
						+ uint16_t(this->currentRedPosition)) % wheelRange,
						Color::SIGNED_RANGE
					)
				);
			}
		}
		this->strip.show();
		this->startTime = micros();
	}
}

void RainbowSpin::setSpeed(const float speed) {
	this->currentTransitionDuration = RainbowSpin::transitionDuration
		* strip.numPixels() / defaultStripLength / speed;
}

void RainbowSpin::setInverse(const uint8_t inverse) {
	this->inverse = inverse;
}
