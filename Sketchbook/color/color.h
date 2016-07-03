#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

class FloatColor;

class Color {
private:
	uint8_t red;
	uint8_t green;
	uint8_t blue;
public:
	enum Precision { SIGNED_RANGE = 128, UNSIGNED_RANGE = 256};

	Color();
	Color(const uint8_t red, const uint8_t green, const uint8_t blue);
	Color(const Color &color);
	Color(const FloatColor &color);

	uint8_t getRed() const;
	uint8_t getGreen() const;
	uint8_t getBlue() const;

	void setRed(const uint8_t red);
	void setGreen(const uint8_t green);
	void setBlue(const uint8_t blue);

	Color operator/(const int divisor) const;

	static Color colorFromWheel(const uint16_t wheelPosition, Precision range);
};

#endif
