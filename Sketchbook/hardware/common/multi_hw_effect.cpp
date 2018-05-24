#include "multi_hw_effect.h"

MultiHWEffect::MultiHWEffect(LPD8806 strip) : LPD8806Effect(strip) {}

MultiHWEffect::MultiHWEffect(LPD8806 strip, uint8_t isMultiControllerEnabled)
    : MultiHWEffect(strip)
{
    this->multiLEDController = isMultiControllerEnabled;
}

void MultiHWEffect::updateStripColor(const Color currentColor,
  const uint8_t redPin, const uint8_t greenPin, const uint8_t bluePin)
{
    if(this->multiLEDController) {
        for(uint16_t i = 0; i < this->strip.numPixels(); i++) {
            this->strip.setPixelColor(i, currentColor);
        }
        this->strip.show();
    } else {
        RGBLedStrip::updateEffect(currentColor, redPin, greenPin, bluePin);
    }
}

void MultiHWEffect::enableSingleLEDController(const uint8_t enable) {
    this->multiLEDController = enable;
}

uint8_t MultiHWEffect::isSingleLEDControllerEnabled() const {
    return this->multiLEDController;
}
