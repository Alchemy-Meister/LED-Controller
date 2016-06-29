#ifndef TIME_BASED_MULTI_HW_EFFECT_H
#define TIME_BASED_MULTI_HW_EFFECT_H

#include "multi_hw_effect.h"

class TimeBasedMultiHWEffect : public MultiHWEffect
{
protected:
  // Time when effect starts in milliseconds.
  uint32_t startTime;
  // Passed time since effect started in milliseconds.
  uint32_t elapsedTime;

public:
  TimeBasedMultiHWEffect(const LPD8806 strip);

  virtual void setSpeed(const float speed) = 0;
};

#endif
