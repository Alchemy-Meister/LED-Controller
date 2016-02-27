#include "spectrum_cycle.h"

const uint32_t SpectrumCycle::transitionDuration = 2000000;
const uint8_t SpectrumCycle::transitionDurationSec = 
	SpectrumCycle::transitionDuration / 1000000;

const Color SpectrumCycle::spectrumColors[colorSize] = {
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

void SpectrumCycle::processEffect(FloatColor &color,
	const float deltaTime)
{
	// Updates cycling elapsed time.
	this->elapsedTime = micros() - this->startTime;

	// IF elapsed time surpasses transitionDuration or initialization
	// flag is enabled.
	if(this->elapsedTime >= SpectrumCycle::transitionDuration || 
		this->spectrumEffectInitialization) 
	{
		// Cleans cycling initialization flag.
		this->spectrumEffectInitialization = 0;
		
		// Updates bi-dimensional array accessing INDEX.
		this->updateColorIndex();
		
		// Retrieves RGB color from the bi-dimensional as color targets.
		this->target = SpectrumCycle::spectrumColors[this->colorIndex];

		// Calculates the transition speed.
		this->calculateSpeed(color, target);

		// Restarts the transition initialization time.
		this->startTime = micros();
	}

	// Calculates new color subtraction after transition.
	this->transition(color, target, deltaTime);
}

SpectrumCycle::SpectrumCycle() {}

void SpectrumCycle::initializeEffect() {
	// Sets initial value of the array access INDEX.
	this->colorIndex = -1;
	// Sets effect INITIALIZATION flag as TRUE.
	this->spectrumEffectInitialization = 1;
	this->elapsedTime = 0;
}

void SpectrumCycle::updateColorIndex() {
	if(this->colorIndex < SpectrumCycle::colorSize - 1) {
		// IF INDEX hasn't reached array's end increases it.
		this->colorIndex++;
	} else {
		// IF INDEX reach array's end re-initializes it.
		this->colorIndex = 0;
	}
}

void SpectrumCycle::calculateSpeed(const FloatColor currentColor,
	const Color targetColor)
{
	this->colorSpeed = Color(
		this->calculateCompSpeed(currentColor.getRed(), targetColor.getRed()),
		this->calculateCompSpeed(currentColor.getGreen(),
			targetColor.getGreen()),
		this->calculateCompSpeed(currentColor.getBlue(),
			targetColor.getBlue()));
}

uint8_t SpectrumCycle::calculateCompSpeed(const float currentComp,
	const uint8_t targetComp)
{
	return ceil(abs(currentComp - targetComp) /
		SpectrumCycle::transitionDurationSec);
}