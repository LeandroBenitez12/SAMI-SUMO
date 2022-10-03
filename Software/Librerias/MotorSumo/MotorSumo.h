#ifndef _MOTORSUMO_H
#define _MOTORSUMO_H
#include "Arduino.h"

class Motor {
private:
  int velocidad;
  int pin_direccion;
  int pin_pwm;

public:
    Motor(int dir, int pwm, int vel);
    void SetVelocidad(int v);
    void Forward();
    void Backward();
    void Stop();
};

#endif