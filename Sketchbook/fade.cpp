#include "fade.h"

const float Fade::fadeDuration	= 3000000;
const float Fade::fadeDurationSeconds = Fade::fadeDuration / 1000000;

const float Fade::offDuration = 1500000;

Fade::Fade() {
	// Sets fade in flag as TRUE.
	this->fadeIn = 0;
}

void Fade::initializeEffect(const FloatColor &color, const uint8_t breath)
{
	// Sets fade in flag as TRUE.
	this->fadeIn = 0;
	// Calculates the face speed for the base color.
	this->calculateSpeed(color);

	// IF true activates its flag.
	if(breath)
		this->breathing = 1;
	else
		this->breathing	= 0;
}

void Fade::calculateSpeed(const FloatColor &color) {
	this->colorSpeed = FloatColor(
		ceil(color.getRed() / Fade::fadeDurationSeconds),
		ceil(color.getGreen() / Fade::fadeDurationSeconds),
		ceil(color.getBlue() / Fade::fadeDurationSeconds)
		);
}

FloatColor Fade::processEffect(FloatColor &color, const float deltaTime) {
	FloatColor transitedColor = FloatColor(color);

	// Updates fade elapsed time.
	this->elapsedTime = micros() - this->startTime;

	// Processes the fade in/out effect parts depending on the flag.
	if(this->breathing) {
		if(this->elapsedTime >= Fade::offDuration) {
			// ELSE set flag to FADE IN.
			this->fadeIn = 1;
			// Restarts the face initialization time.
			this->startTime = micros();
		}
	} else {
		if(this->fadeIn) {
			// Processes the FADE IN effect part.

			// Calculates new color addition after transition.
			transitedColor = this->transition(
				color, Color(255, 255, 255), deltaTime);
			
			// IF elapsed time surpasses fade duration.
			if(this->elapsedTime >= Fade::fadeDuration) {
				// Sets the flag to FADE OUT.
				this->fadeIn = 0;
				// Restarts the face initialization time.
				this->startTime = micros();
			}
		} else {
			// Processes the FADE OUT effect part.

			// Calculates new color subtraction after transition.
			transitedColor = this->transition(color, Color(0, 0, 0), deltaTime);

			// IF elapsed time surpasses fade duration.
			if(this->elapsedTime >= Fade::fadeDuration) {
				// IF the breathing flag is enabled.
				if(this->breathing) {
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

	return transitedColor;
}