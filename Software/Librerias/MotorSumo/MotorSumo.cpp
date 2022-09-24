#include "MotorSumo.h"

Motor::Motor(int dir, int pwm, int vel)
{
    pin_direccion = dir;
    pin_pwm = pwm;
    velocidad = vel;

    pinMode(pin_direccion, OUTPUT);
    pinMode(pin_pwm, OUTPUT);
}
void Motor::SetVelocidad(int v)
{
    velocidad = v;
}
void Motor::Forward()
{
    digitalWrite(pin_direccion, HIGH);
    analogWrite(pin_pwm, velocidad);
}
void Motor::Backward()
{
    digitalWrite(pin_direccion, LOW);
    analogWrite(pin_pwm, velocidad);
}
void Motor::Stop()
{
    digitalWrite(pin_direccion, LOW);
    analogWrite(pin_pwm, 0);
}