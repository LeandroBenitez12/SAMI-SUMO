#include "SumoEngineController.h"

Engine::Engine(int dir, int pwm)
{
    pinDirection = dir;
    pinPwm = pwm;

    pinMode(pinDirection, OUTPUT);
    pinMode(pinPwm, OUTPUT);
}
void Engine::Forward(int speed)
{
    speed = speed;
    digitalWrite(pinDirection, HIGH);
    analogWrite(pinPwm, speed);
}
void Engine::Backward(int speed)
{
    speed = speed;
    analogWrite(pinPwm, 0);
    digitalWrite(pinDirection, LOW);
    analogWrite(pinPwm, speed);
}
void Engine::Stop()
{
    digitalWrite(pinDirection, LOW);
    analogWrite(pinPwm, 0);
}


EngineController::EngineController(int dirRight, int pwmRight, int dirLeft, int pwmLeft)
{
    pinDirectionRight = dirRight;
    pinPwmRihgt = pwmRight;
    pinDirectionLeft = dirLeft;
    pinPwmLeft = pwmLeft;
    engineRight = new Engine(pinDirectionRight, pinPwmRihgt);
    engineLeft = new Engine(pinDirectionLeft, pinPwmLeft);
}
void EngineController::Forward(int pwmRight, int pwmLeft)
{
    speedRight = pwmRight;
    speedLeft = pwmLeft;
    engineRight->Forward(speedRight);
    engineLeft->Forward(speedLeft);
}
void EngineController::Backward(int pwmRight, int pwmLeft)
{
    speedRight = pwmRight;
    speedLeft = pwmLeft;
    engineRight->Backward(speedRight);
    engineLeft->Backward(speedLeft);
}
void EngineController::Right(int pwmRight, int pwmLeft)
{
    speedRight = pwmRight;
    speedLeft = pwmLeft;
    engineRight->Backward(speedRight);
    engineLeft->Forward(speedLeft);
}
void EngineController::Left(int pwmRight, int pwmLeft)
{
    speedRight = pwmRight;
    speedLeft = pwmLeft;
    engineRight->Forward(speedRight);
    engineLeft->Backward(speedLeft);
}
void EngineController::Stop()
{
    engineRight->Stop();
    engineLeft->Stop();
}