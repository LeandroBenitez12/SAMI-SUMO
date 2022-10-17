#ifndef _TATAMI_H
#define _TATAMI_H
#include "Arduino.h"

class Tatami 
{
private:
  int pin;
  int minimoTatami = 300;
  int maximoTatami = 700;
  int n = 3;

public:
  Tatami(int p);
  float TatamiRead();
};

#endif