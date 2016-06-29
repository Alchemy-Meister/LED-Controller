#ifndef LPD8806_EFFECT_H
#define LPD8806_EFFECT_H

#include "lib/LPD8806.h"

class LPD8806Effect
{
protected:
	static const uint8_t MAX_BRIGTHNESS = 127;

	LPD8806 strip;

public:
	LPD8806Effect(const LPD8806 strip);

	void setStrip(const LPD8806 strip);
	LPD8806 getStrip() const;
};

#endif
