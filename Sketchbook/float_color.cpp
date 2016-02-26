#include "float_color.h"

FloatColor::FloatColor() {
	this->red = 0;
	this->green = 0;
	this->blue = 0;
}

FloatColor::FloatColor(const float red, const float green, const float blue) {
	this->red = red;
	this->green = green;
	this->blue = blue;
}
FloatColor::FloatColor(const FloatColor &color) {
	this->red = color.getRed();
	this->green = color.getGreen();
	this->blue = color.getBlue();
}

FloatColor::FloatColor(const Color &color) {
	this->red = color.getRed();
	this->green = color.getGreen();
	this->blue = color.getBlue();
}

float FloatColor::getRed() const {
	return this->red;
}

float FloatColor::getGreen() const {
	return this->green;
}

float FloatColor::getBlue() const {
	return this->blue;
}

float FloatColor::getComponent(RGB component) const {
	switch(component) {
		case RED:
			return this->red;
		case GREEN:
			return this->green;
		case BLUE:
			return this->blue;
	}
}

void FloatColor::setRed(const float red) {
	this->red = red;
}

void FloatColor::setGreen(const float green) {
	this->green = green;
}

void FloatColor::setBlue(const float blue) {
	this->blue = blue;
}

void FloatColor::setComponent(RGB component, const float value) {
	switch(component) {
		case RED:
			this->red = value;
			break;
		case GREEN:
			this->green = value;
			break;
		case BLUE:
			this->blue = value;
			break;
	}
}

void FloatColor::setColors(const FloatColor &color) {
	this->red = color.getRed();
	this->green = color.getGreen();
	this->blue = color.getBlue();
}

void FloatColor::setColors(const Color &color) {
	this->red = color.getRed();
	this->green = color.getGreen();
	this->blue = color.getBlue();
}

// This function Check if the input value exceeds the target value.
float FloatColor::colorUpLimiter(const float color, const uint8_t limiter) {
	// IF input surpasses the target returns the target value.
	if(color > limiter)
		return limiter;
	return color;
}

// This function Check if the input value exceeds the target value.
float FloatColor::colorDownLimiter(const float color, const uint8_t limiter) {
	// IF input surpasses the target returns the target value.
	if(color < limiter)
		return limiter;
	return color;
}

// This function returns a boolean flag to check if the input needs to 
// increment or decrement to reach the target value.
uint8_t FloatColor::componentIsIncrement(const float color,
	const uint8_t target)
{
	if(color < target)
		return 1;
	return 0;
}