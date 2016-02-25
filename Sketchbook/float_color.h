#ifndef FLOAT_COLOR_H
#define FLOAT_COLOR_H

#include "color.h"

class FloatColor
{
	private:
		float red;
		float green;
		float blue;
	public:
		enum RGB { RED, GREEN, BLUE };

		FloatColor();
		FloatColor(const float red, const float green, const float blue);
		FloatColor(const FloatColor &color);
		FloatColor(const Color &color);

		float getRed() const;
		float getGreen() const;
		float getBlue() const;

		float getComponent(FloatColor::RGB component) const;

		void setRed(const float red);
		void setGreen(const float green);
		void setBlue(const float blue);

		void setComponent(FloatColor::RGB component, const float value);

		void setColors(const FloatColor &color);
		void setColors(const Color &color);

		static uint8_t colorUpLimiter(const float color, const uint8_t limiter);
		static uint8_t colorDownLimiter(const float color, 
			const uint8_t limiter);
		static uint8_t componentIsIncrement(const uint8_t color,
			const uint8_t target);
};

#endif