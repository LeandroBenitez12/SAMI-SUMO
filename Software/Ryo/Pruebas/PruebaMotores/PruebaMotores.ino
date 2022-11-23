#include <Engineesp32.h>

// Variables y constantes para los motores
#define PIN_ENGINE_IN1_RIGHT 21
#define PIN_ENGINE_IN2_RIGHT 19
#define PIN_ENGINE_IN1_LEFT 22
#define PIN_ENGINE_IN2_LEFT 23
int rightSpeed = 100;
int leftSpeed = 100;

EngineESP32 *Ryo = new EngineESP32(PIN_ENGINE_IN1_RIGHT, PIN_ENGINE_IN2_RIGHT, PIN_ENGINE_IN1_LEFT, PIN_ENGINE_IN2_LEFT);

void setup(){

}

void loop(){
    Ryo->Forward(rightSpeed, leftSpeed);
    delay(3000);
    Ryo->Backward(rightSpeed, leftSpeed);
    delay(3000);
    Ryo->Rigth(rightSpeed, leftSpeed);
    delay(3000);
    Ryo->Left(rightSpeed, leftSpeed);
    delay(3000);
    Ryo->Stop();
    delay(3000);
    
}
