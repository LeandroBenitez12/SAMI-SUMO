#ifndef _SHARP_H
#define _SHARP_H
#include "Arduino.h"

class Sharp
{
private:
  int pin;
  int n = 3;

public:
  Sharp(int p);
  double SharpDist();
};

#endif