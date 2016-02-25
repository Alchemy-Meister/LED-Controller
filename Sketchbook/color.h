#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

class Color
{
	private:
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	public:
		Color();
		Color(const uint8_t red, const uint8_t green, const uint8_t blue);
		Color(const Color &color);

		uint8_t getRed() const;
		uint8_t getGreen() const;
		uint8_t getBlue() const;

		void setRed(const uint8_t red);
		void setGreen(const uint8_t green);
		void setBlue(const uint8_t blue);
};

#endif