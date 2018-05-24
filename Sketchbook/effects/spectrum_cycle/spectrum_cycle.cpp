#include "spectrum_cycle.h"

const uint32_t SpectrumCycle::transitionDuration = 2000000;

// Spectrum colors values for multi led controller HW.
const Color SpectrumCycle::spectrumColors[colorSize] = {
    Color(255, 0, 0),   // Color transition.
    Color(255, 0, 0),   // Idle color.
    Color(255, 153, 0), // Color transition.
    Color(255, 153, 0), // Idle color.
    Color(255, 255, 0), // Repeats...
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

SpectrumCycle::SpectrumCycle(const LPD8806 strip)
: DynamicTimeBasedMultiHWEffect(strip)
{
    this->currentTransitionDuration = SpectrumCycle::transitionDuration;
}

void SpectrumCycle::initializeEffect() {
    // Sets initial value of the array access INDEX.
    this->colorIndex = -1;
    // Sets effect INITIALIZATION flag as TRUE.
    this->spectrumEffectInitialization = 1;
    this->elapsedTime = 0;
}

void SpectrumCycle::setSpeed(const float speed) {
    this->currentTransitionDuration = SpectrumCycle::transitionDuration / speed;
}

void SpectrumCycle::processEffect(FloatColor &color,
    const float deltaTime)
{
    // Updates cycling elapsed time.
    this->elapsedTime = micros() - this->startTime;

    // IF elapsed time surpasses transitionDuration or initialization
    // flag is enabled.
    if(this->elapsedTime >= this->currentTransitionDuration ||
        this->spectrumEffectInitialization)
    {

        // Cleans cycling initialization flag.
        this->spectrumEffectInitialization = 0;

        // Updates bi-dimensional array accessing INDEX.
        this->updateColorIndex();

        // Retrieves RGB color from the bi-dimensional as color targets.
        if(this->multiLEDController) {
            this->target = (SpectrumCycle::spectrumColors[this->colorIndex])
                / (RGBLedStrip::MAX_BRIGTHNESS / LPD8806Effect::MAX_BRIGTHNESS);
        } else {
            this->target = SpectrumCycle::spectrumColors[this->colorIndex];
        }

        // Calculates the transition speed.
        this->calculateSpeed(color, this->target);

        // Restarts the transition initialization time.
        this->startTime = micros();
    }

    // Calculates new color subtraction after transition.
    this->transition(color, this->target, deltaTime);
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
    this->colorSpeed = ColorSpeed(
        this->calculateCompSpeed(currentColor.getRed(), targetColor.getRed()),
        this->calculateCompSpeed(currentColor.getGreen(),
            targetColor.getGreen()),
        this->calculateCompSpeed(currentColor.getBlue(),
            targetColor.getBlue()));
}

uint16_t SpectrumCycle::calculateCompSpeed(const float currentComp,
    const uint8_t targetComp)
{
    return ceil(abs(currentComp - targetComp) /
        (this->currentTransitionDuration / 1000000.0));
}
