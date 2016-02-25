#include "color.h"

Color::Color() {
	this->red = 0;
	this->green = 0;
	this->blue = 0;
}

Color::Color(const uint8_t red, const uint8_t green, const uint8_t blue) {
	this->red = red;
	this->green = green;
	this->blue = blue;
}
Color::Color(const Color &color) {
	this->red = color.red;
	this->green = color.green;
	this->blue = color.blue;
}

uint8_t Color::getRed() const {
	return this->red;
}

uint8_t Color::getGreen() const {
	return this->green;
}

uint8_t Color::getBlue() const {
	return this->blue;
}

void Color::setRed(const uint8_t red) {
	this->red = red;
}

void Color::setGreen(const uint8_t green) {
	this->green = green;
}

void Color::setBlue(const uint8_t blue) {
	this->blue = blue;
}