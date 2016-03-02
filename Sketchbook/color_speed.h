#ifndef COLOR_SPEED_H
#define COLOR_SPEED_H

#include <stdint.h>

class ColorSpeed
{
	private:
		uint16_t red;
		uint16_t green;
		uint16_t blue;
	public:
		ColorSpeed();
		ColorSpeed(const uint16_t red, const uint16_t green, const uint16_t blue);
		ColorSpeed(const ColorSpeed &colorSpeed);

		uint16_t getRedSpeed() const;
		uint16_t getGreenSpeed() const;
		uint16_t getBlueSpeed() const;

		void setRedSpeed(const uint16_t redSpeed);
		void setGreenSpeed(const uint16_t greenSpeed);
		void setBlueSpeed(const uint16_t blueSpeed);
};

#endif