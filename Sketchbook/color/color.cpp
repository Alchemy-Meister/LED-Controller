#include "color.h"
#include "float_color.h"

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

Color::Color(const FloatColor &color) {
    this->red = color.getRed();
    this->green = color.getGreen();
    this->blue = color.getBlue();
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

Color Color::operator/(const int divisor) const {
    return Color(
        this->red / divisor,
        this->green/ divisor,
        this->blue / divisor
    );
}

Color Color::colorFromWheel(const uint16_t wheelPosition, Precision range) {
    Color returnColor;
    switch(wheelPosition / range) {
        case 0:
            returnColor.setRed((range - 1) - wheelPosition % range);
            returnColor.setGreen(wheelPosition % range);
            returnColor.setBlue(0);
            break;
        case 1:
            returnColor.setRed(0);
            returnColor.setGreen((range - 1) - wheelPosition % range);
            returnColor.setBlue(wheelPosition % range);
            break;
        case 2:
            returnColor.setRed(wheelPosition % range);
            returnColor.setGreen(0);
            returnColor.setBlue((range - 1) - wheelPosition % range);
            break;
    }
    return returnColor;
}
