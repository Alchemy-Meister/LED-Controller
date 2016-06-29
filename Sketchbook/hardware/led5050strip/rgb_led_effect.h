#ifndef RGB_LED_EFFECT_H
#define RGB_LED_EFFECT_H

#include <stdint.h>
#include <Arduino.h>
#include "color/color.h"

class RGBLedStrip {
protected:
	static const uint8_t MAX_BRIGTHNESS = 255;

public:
	void updateEffect(const Color currentColor, const uint8_t redPin,
		const uint8_t greenPin, const uint8_t bluePin);
};

#endif
