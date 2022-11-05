#ifndef _SHARP_H
#define _SHARP_H
#include "Arduino.h"

class Sharp
{
private:
  int pin;
<<<<<<< HEAD

public:
  Sharp(int p);
  double SharpDist(int n);
=======
  int n = 3;

public:
  Sharp(int p);
  double SharpDist();
>>>>>>> 84d439c4c9d53cc8b836e205e0ca530ee9426de7
};

#endif