#include "rgb_led_effect.h"

void RGBLedStrip::updateEffect(const Color currentColor, const uint8_t redPin,
    const uint8_t greenPin, const uint8_t bluePin)
{
    analogWrite(redPin, currentColor.getRed());
    analogWrite(greenPin, currentColor.getGreen());
    analogWrite(bluePin, currentColor.getBlue());
}
