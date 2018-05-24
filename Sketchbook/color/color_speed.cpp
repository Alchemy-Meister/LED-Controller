#include "color_speed.h"

ColorSpeed::ColorSpeed() {
    this->red = 0;
    this->green = 0;
    this->blue = 0;
}

ColorSpeed::ColorSpeed(const uint16_t red, const uint16_t green,
    const uint16_t blue)
{
    this->red = red;
    this->green = green;
    this->blue = blue;
}

ColorSpeed::ColorSpeed(const ColorSpeed &colorSpeed) {
    this->red = colorSpeed.red;
    this->green = colorSpeed.green;
    this->blue = colorSpeed.blue;
}

uint16_t ColorSpeed::getRedSpeed() const {
    return this->red;
}

uint16_t ColorSpeed::getGreenSpeed() const {
    return this->green;
}

uint16_t ColorSpeed::getBlueSpeed() const {
    return this->blue;
}

void ColorSpeed::setRedSpeed(const uint16_t redSpeed) {
    this->red = red;
}

void ColorSpeed::setGreenSpeed(const uint16_t greenSpeed) {
    this->green = green;
}

void ColorSpeed::setBlueSpeed(const uint16_t blueSpeed) {
    this->blue = blue;
}
