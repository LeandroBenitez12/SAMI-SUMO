#include <Button.h>
#include <MotorSumo.h>
#include <Tatami.h>
#include <Sharp.h>

#define DEBUG_TATAMI 1
#define DEBUG_movimientos 1
//Sensors de tatami
#define PIN_SENSOR_TATAMI_IZQ A0
#define PIN_SENSOR_TATAMI_DER A1
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 300
int n = 3;

//sensor de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO A3
#define PIN_SENSOR_DISTANCIA_IZQUIERDO A4
#define RIVAL 30
int distSharpRigh;
int distSharpLeft;

// motor
#define PIN_MOTOR_MR1 11 //DIR
#define PIN_MOTOR_MR2PWM 10 //PWM
#define PIN_MOTOR_ML1 9 //DIR
#define PIN_MOTOR_ML2PWM 6 //PWM
#define PIN_BUTTON_START 2
#define PIN_BUTTON_STRATEGY 3  //te ponen 
#define PIN_BUZZER 5
#define SEARCH_SPEED 100
#define ATTACK_SPEED 250
#define AVERAGE_SPEED 200;
int righSpeed = 200;
int leftSpeed = 200;





Motor *mDer = new Motor(PIN_MOTOR_MR1, PIN_MOTOR_MR2PWM, righSpeed);
Motor *mIzq = new Motor(PIN_MOTOR_ML1, PIN_MOTOR_ML2PWM, leftSpeed);

Tatami *rightTatami = new Tatami(PIN_SENSOR_TATAMI_DER);
Tatami *LeftTatami = new Tatami(PIN_SENSOR_TATAMI_IZQ);

Sharp *sharpRight = new Sharp(PIN_SENSOR_DISTANCIA_DERECHO);
Sharp *sharpLeft = new Sharp(PIN_SENSOR_DISTANCIA_IZQUIERDO);

Button *strategy = new  Button(PIN_BUTTON_STRATEGY);
Button *start = new  Button(PIN_BUTTON_START);

void forward() //voy hacia adelante
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Forward();
  mIzq->Forward();
}

void backward() //voy hacia atras
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Backward();
  mIzq->Backward();
}
 
void left() //giro a la izquierda
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Forward();
  mIzq->Backward();
}


void right() //giro a la derecha
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Backward();
  mIzq->Forward();
}

void stopMotor() //freno
{
  mDer->SetVelocidad(0);
  mIzq->SetVelocidad(0);
  mDer->Stop();
  mIzq->Stop();
}

enum strategy1{
  STANDBY,
  SEARCH,
  TURN_RIGHT,
  TURN_LEFT,
  TATAMI_LIMIT,
  ATTACK
};
int strategy1 = STANDBY;

void movementLogic()
{
    switch (strategy1)
    {
    case STANDBY:
    {
        bool boton_start = start->GetIsPress();
        if (!boton_start) 
        {
          delay(5000);
          strategy1 = SEARCH;
        } 
        else 
        {
          stopMotor();
        }
        break;
      }

    case SEARCH:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < 250 || righTatamiRead < 250) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh <= RIVAL && leftTatamiRead > RIVAL) strategy1 = TURN_RIGHT;
        if(distSharpRigh > RIVAL && leftTatamiRead <= RIVAL) strategy1 = TURN_LEFT;
        if(distSharpRigh <= RIVAL && leftTatamiRead <= RIVAL) strategy1 = ATTACK;
    }

    case TURN_RIGHT:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < 250 || righTatamiRead < 250) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && leftTatamiRead > RIVAL) strategy1 = SEARCH;
        if(distSharpRigh > RIVAL && leftTatamiRead <= RIVAL) strategy1 = TURN_LEFT;
        if(distSharpRigh <= RIVAL && leftTatamiRead <= RIVAL) strategy1 = ATTACK;
    }

    case TURN_LEFT:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        left();
        if(leftTatamiRead < 250 || righTatamiRead < 250) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && leftTatamiRead > RIVAL) strategy1 = SEARCH;
        if(distSharpRigh <= RIVAL && leftTatamiRead > RIVAL) strategy1 = TURN_RIGHT;
        if(distSharpRigh <= RIVAL && leftTatamiRead <= RIVAL) strategy1 = ATTACK;
    }

    case ATTACK:
    {
        righSpeed = ATTACK_SPEED + (distSharpRigh * (-2));
        leftSpeed = ATTACK_SPEED + (leftTatamiRead * (-2));
        forward();
        if(leftTatamiRead < 250 || righTatamiRead < 250) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && leftTatamiRead > RIVAL) strategy1 = SEARCH;
        if(distSharpRigh <= RIVAL && leftTatamiRead > RIVAL) strategy1 = TURN_RIGHT;
        if(distSharpRigh > RIVAL && leftTatamiRead <= RIVAL) strategy1 = TURN_LEFT;

    }

    case TATAMI_LIMIT: 
    {
    righSpeed = AVERAGE_SPEED;
    leftSpeed = AVERAGE_SPEED;
    backward();
    delay(300);
    strategy1 = SEARCH;
     break;
    }

    }

}

void setup()
{
    Serial.begin(9600);
}

void loop() {

  distSharpRigh = sharpRight->SharpDist(n);
  distSharpLeft = sharpLeft->SharpDist(n);
  righTatamiRead = rightTatami->TatamiRead(n);
  leftTatamiRead = LeftTatami->TatamiRead(n);
  movementLogic();
}