#include <SumoEngineController.h>
// Variables y constantes para los motores
#define PIN_ENGINE_DIR_LEFT 22 //DIR
#define PIN_ENGINE_PWM_LEFT 21 //PWM
#define PIN_ENGINE_DIR_RIGHT 19 //DIR
#define PIN_ENGINE_PWM_RIGHT 18 //PWM
int rightSpeed = 255;
int leftSpeed = 255;
#define TICK_TURN_FRONT 59// 12 volt 115 45°
#define TICK_TURN_SIDE 95// 12 volt 168 90°
#define TICK_SHORT_BACK_TURN 115// 12 volt 200 110°
#define TICK_LONG_BACK_TURN 120// 12 volt 135°
EngineController *sami = new EngineController(PIN_ENGINE_DIR_RIGHT, PIN_ENGINE_PWM_RIGHT, PIN_ENGINE_DIR_LEFT, PIN_ENGINE_PWM_LEFT);

void setup(){

}

void loop(){
    sami->Right(rightSpeed, leftSpeed);
    delay(TICK_TURN_FRONT);
    sami->Stop();
    delay(3000);
    sami->Right(rightSpeed, leftSpeed);
    delay(TICK_TURN_SIDE);
    sami->Stop();
    delay(3000);
    sami->Right(rightSpeed, leftSpeed);
    delay(TICK_SHORT_BACK_TURN);
    sami->Stop();
    delay(3000);
    sami->Right(rightSpeed, leftSpeed);
    delay(TICK_LONG_BACK_TURN);
    sami->Stop();
    delay(3000);
}
