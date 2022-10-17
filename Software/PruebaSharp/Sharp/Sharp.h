#ifndef _SHARP_H
#define _SHARP_H
#include "Arduino.h"

class Sharp
{
private:
  int pin;

public:
  Sharp(int p);
  double SharpDist(int n);
};

#endif