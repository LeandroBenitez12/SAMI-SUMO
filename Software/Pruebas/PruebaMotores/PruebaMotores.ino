#include <SumoEngineController.h>
// Variables y constantes para los motores
#define PIN_ENGINE_DIR_LEFT 22 //DIR
#define PIN_ENGINE_PWM_LEFT 21 //PWM
#define PIN_ENGINE_DIR_RIGHT 19 //DIR
#define PIN_ENGINE_PWM_RIGHT 18 //PWM
#define PWM_CHANNEL_RIGHT 12
#define PWM_CHANNEL_LEFT 11
int rightSpeed = 100;
int leftSpeed = 255;

EngineController *sami = new EngineController(PIN_ENGINE_DIR_RIGHT, PIN_ENGINE_PWM_RIGHT, PWM_CHANNEL_RIGHT, PIN_ENGINE_DIR_LEFT, PIN_ENGINE_PWM_LEFT, PWM_CHANNEL_LEFT);

void setup(){

}

void loop(){
    sami->Forward(rightSpeed, leftSpeed);
    delay(3000);
    sami->Backward(rightSpeed, leftSpeed);
    delay(3000);
    sami->Right(rightSpeed, leftSpeed);
    delay(3000);
    sami->Left(rightSpeed, leftSpeed);
    delay(3000);
    sami->Stop();
    delay(3000);
}
