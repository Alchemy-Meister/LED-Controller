#include "LPD8806_effect.h"

LPD8806Effect::LPD8806Effect(const LPD8806 strip) {
	this->strip = strip;
}

void LPD8806Effect::setStrip(const LPD8806 strip) {
	this->strip = strip;
}

LPD8806 LPD8806Effect::getStrip() const {
	return this->strip;
}