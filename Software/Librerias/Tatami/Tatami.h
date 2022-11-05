#ifndef _TATAMI_H
#define _TATAMI_H
#include "Arduino.h"

class Tatami 
{
private:
  int pin;
  int minimoTatami = 300;
  int maximoTatami = 700;
<<<<<<< HEAD

public:
  Tatami(int p);
  float TatamiRead(int n);
=======
  int n = 3;

public:
  Tatami(int p);
  float TatamiRead();
>>>>>>> 84d439c4c9d53cc8b836e205e0ca530ee9426de7
};

#endif