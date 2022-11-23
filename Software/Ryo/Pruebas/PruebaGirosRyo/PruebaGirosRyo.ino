#include <Engineesp32.h>
// Variables y constantes para los motores
#define PIN_ENGINE_IN1_RIGHT 19
#define PIN_ENGINE_IN2_RIGHT 21
#define PIN_ENGINE_IN1_LEFT 22
#define PIN_ENGINE_IN2_LEFT 23
int rightSpeed = 255;
int leftSpeed = 255;
#define TICK_TURN_FRONT 29// 45g
#define TICK_TURN_SIDE 46// 90g
#define TICK_SHORT_BACK_TURN 67// 110g
#define TICK_LONG_BACK_TURN 83?// 135g
EngineESP32 *Ryo = new EngineESP32(PIN_ENGINE_IN1_RIGHT, PIN_ENGINE_IN2_RIGHT, PIN_ENGINE_IN1_LEFT, PIN_ENGINE_IN2_LEFT);

void setup(){

}

void loop(){
    Ryo->Stop();
    delay(2000);
    Ryo->Right(rightSpeed, leftSpeed);
    delay(TICK_TURN_FRONT);
    Ryo->Stop();
    delay(3000);
    Ryo->Right(rightSpeed, leftSpeed);
    delay(TICK_TURN_SIDE);
    Ryo->Stop();
    delay(3000);
    Ryo->Right(rightSpeed, leftSpeed);
    delay(TICK_SHORT_BACK_TURN);
    Ryo->Stop();
    delay(3000);
    Ryo->Right(rightSpeed, leftSpeed);
    delay(TICK_LONG_BACK_TURN);
    Ryo->Stop();
    delay(3000);
}
