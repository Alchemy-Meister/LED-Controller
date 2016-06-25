#include "fade.h"

const uint32_t Fade::fadeDuration	= 3000000;
const uint32_t Fade::offDuration = 1500000;

Fade::Fade(const LPD8806 strip) {
	this->breathSpeed = 1;
	this->fadeSpeed = 1;
	this->currentFadeDuration = Fade::fadeDuration;
	this->currentOffDuration = Fade::offDuration;
	this->strip = strip;
}

void Fade::initializeEffect(const FloatColor &color, const uint8_t breath)
{
	// Sets fade in flag as FALSE.
	this->fadeIn = 0;

	this->baseColor = FloatColor(color);

	// IF true activates breath flag and updates its speed.
	if(breath) {
		this->breath = 1;
		this->currentFadeDuration = Fade::fadeDuration / this->breathSpeed;
		this->currentOffDuration = Fade::offDuration / this->breathSpeed;
	} else {
		this->breath = 0;
		this->currentFadeDuration = Fade::fadeDuration / this->fadeSpeed;
		this->currentOffDuration = Fade::offDuration / this->fadeSpeed;
	}

	// Calculates the face speed for the base color.
	this->calculateSpeed(color);
}

void Fade::setStartTime(const uint32_t currentTime) {
	this->startTime = currentTime;
}

void Fade::setSpeed(const float speed) {
	if(this->breath) {
		this->breathSpeed = speed;
		this->currentFadeDuration = Fade::fadeDuration / this->breathSpeed;
		this->currentOffDuration = Fade::offDuration / this->breathSpeed;
	} else {
		this->fadeSpeed = speed;
		this->currentFadeDuration = Fade::fadeDuration / this->fadeSpeed;
		this->currentOffDuration = Fade::offDuration / this->fadeSpeed;
	}
	this->calculateSpeed(this->baseColor);
}

void Fade::calculateSpeed(const FloatColor &color) {
	uint8_t duration = this->currentFadeDuration / 1000000;
	this->colorSpeed = ColorSpeed(
		ceil(color.getRed() / duration),
		ceil(color.getGreen() / duration),
		ceil(color.getBlue() / duration)
		);
}

void Fade::processEffect(FloatColor &color, const float deltaTime) {


	// Updates fade elapsed time.
	this->elapsedTime = micros() - this->startTime;

	// Processes the fade in/out effect parts depending on the flag.
	if(this->breathing) {
		if(this->elapsedTime >= this->currentOffDuration) {
			// ELSE set flag to FADE IN.
			this->fadeIn = 1;
			this->breathing = 0;
			// Restarts the face initialization time.
			this->startTime = micros();
		}
	} else {
		if(this->fadeIn) {
			// Processes the FADE IN effect part.

			// Calculates new color addition after transition.
			if(this->singleLEDController) {
				this->transition(color,
					Color(
						this->LPD8806_MAX,
						this->LPD8806_MAX,
						this->LPD8806_MAX
					), deltaTime
				);
			} else {
				this->transition(color,
					Color(
						this->MULTI_MAX,
						this->MULTI_MAX,
						this->MULTI_MAX
					), deltaTime);
			}
			
			// IF elapsed time surpasses fade duration.
			if(this->elapsedTime >= this->currentFadeDuration) {
				// Sets the flag to FADE OUT.
				this->fadeIn = 0;
				// Restarts the face initialization time.
				this->startTime = micros();
			}
		} else {
			// Processes the FADE OUT effect part.

			// Calculates new color subtraction after transition.
			this->transition(color, Color(0, 0, 0), deltaTime);

			// IF elapsed time surpasses fade duration.
			if(this->elapsedTime >= this->currentFadeDuration) {
				// IF the breathing flag is enabled.
				if(this->breath) {
					// ELSE set flag to BREATHING.
					this->breathing = 1;
					// Restarts the face initialization time.
					this->startTime = micros();
				} else {
					// ELSE set flag to FADE IN.
					this->fadeIn = 1;
					// Restarts the face initialization time.
					this->startTime = micros();
				}
			}
		}
	}
}