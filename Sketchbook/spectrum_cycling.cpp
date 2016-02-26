#include "spectrum_cycling.h"

const float SpectrumCycling::transitionDuration = 2000000;
const float SpectrumCycling::transitionDurationSec = 
	SpectrumCycling::transitionDuration / 1000000;

const Color SpectrumCycling::spectrumColors[colorSize] = {
			Color(255, 0, 0),	// Color transition.
			Color(255, 0, 0),	// Idle color.
			Color(255, 153, 0),	// Color transition.
			Color(255, 153, 0),	// Idle color.
			Color(255, 255, 0),	// Repeats...
			Color(255, 255, 0),
			Color(0, 255, 0),
			Color(0, 255, 0),
			Color(0, 255, 255),
			Color(0, 255, 255),
			Color(0, 0, 255),
			Color(0, 0, 255),
			Color(153, 0, 255),
			Color(153, 0, 255),
			Color(255, 0, 255),
			Color(255, 0, 255)
		};

void SpectrumCycling::processEffect(FloatColor &color,
	const float deltaTime)
{
	// Updates cycling elapsed time.
	this->elapsedTime = micros() - this->startTime;

	// IF elapsed time surpasses transitionDuration or initialization
	// flag is enabled.
	if(this->elapsedTime >= SpectrumCycling::transitionDuration || 
		this->spectrumEffectInitialization) 
	{
		// Cleans cycling initialization flag.
		this->spectrumEffectInitialization = 0;
		
		// Updates bi-dimensional array accessing INDEX.
		this->updateColorIndex();
		
		// Retrieves RGB color from the bi-dimensional as color targets.
		this->target = SpectrumCycling::spectrumColors[this->colorIndex];

		// Calculates the transition speed.
		this->calculateSpeed(color, target);

		// Restarts the transition initialization time.
		this->startTime = micros();
	}

	// Calculates new color subtraction after transition.
	this->transition(color, target, deltaTime);
}

SpectrumCycling::SpectrumCycling() {}

void SpectrumCycling::initializeEffect() {
	// Sets initial value of the array access INDEX.
	this->colorIndex = -1;
	// Sets effect INITIALIZATION flag as TRUE.
	this->spectrumEffectInitialization = 1;
	this->elapsedTime = 0;
}

void SpectrumCycling::setInitializationFlag(const uint8_t initialize) {
	this->spectrumEffectInitialization = initialize;
}

void SpectrumCycling::updateColorIndex() {
	if(this->colorIndex < SpectrumCycling::colorSize - 1) {
		// IF INDEX hasn't reached array's end increases it.
		this->colorIndex++;
	} else {
		// IF INDEX reach array's end re-initializes it.
		this->colorIndex = 0;
	}
}

void SpectrumCycling::calculateSpeed(const FloatColor currentColor,
	const Color targetColor)
{
	this->colorSpeed = Color(
		this->calculateCompSpeed(currentColor.getRed(), targetColor.getRed()),
		this->calculateCompSpeed(currentColor.getGreen(),
			targetColor.getGreen()),
		this->calculateCompSpeed(currentColor.getBlue(),
			targetColor.getBlue()));
}

uint8_t SpectrumCycling::calculateCompSpeed(const float currentComp,
	const uint8_t targetComp)
{
	return ceil(abs(currentComp - targetComp) /
		SpectrumCycling::transitionDurationSec);
}