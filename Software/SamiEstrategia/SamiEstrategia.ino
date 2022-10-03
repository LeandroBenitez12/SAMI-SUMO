#include <Button.h>
#include <MotorSumo.h>
#include <Tatami.h>
#include <Sharp.h>

#define DEBUG 1
#define DEBUG_STATE 1
#define TICK_DEBUG 500
unsigned long tiempo_actual = 0;


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
//-------------------------------------------------------------

Motor *mDer = new Motor(PIN_MOTOR_MR1, PIN_MOTOR_MR2PWM, righSpeed);
Motor *mIzq = new Motor(PIN_MOTOR_ML1, PIN_MOTOR_ML2PWM, leftSpeed);

Tatami *rightTatami = new Tatami(PIN_SENSOR_TATAMI_DER);
Tatami *LeftTatami = new Tatami(PIN_SENSOR_TATAMI_IZQ);

Sharp *sharpRight = new Sharp(PIN_SENSOR_DISTANCIA_DERECHO);
Sharp *sharpLeft = new Sharp(PIN_SENSOR_DISTANCIA_IZQUIERDO);

Button *strategy = new  Button(PIN_BUTTON_STRATEGY);
Button *start = new  Button(PIN_BUTTON_START);

//-------------------------------------------------------------
void forward()
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Forward();
  mIzq->Forward();
}

void backward()
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Backward();
  mIzq->Backward();
}
 
void left()
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Forward();
  mIzq->Backward();
}


void right()
{
  mDer->SetVelocidad(righSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Backward();
  mIzq->Forward();
}

void stopMotor()
{
  mDer->SetVelocidad(0);
  mIzq->SetVelocidad(0);
  mDer->Stop();
  mIzq->Stop();
}
//-------------------------------------------------------------

enum strategy1{
  STANDBY,
  SEARCH,
  TURN_RIGHT,
  TURN_LEFT,
  TATAMI_LIMIT,
  ATTACK
};
int strategy1 = STANDBY;

void strategy()
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
//-------------------------------------------------------------

void printSensors()
{
  if (millis() > tiempo_actual + TICK_DEBUG)
        {
          Serial.print("Right tatami: ");
          Serial.print(righTatamiRead);
          Serial.print("  //  ");
          Serial.print("Left tatami: ");
          Serial.println(leftTatamiRead);
          Serial.print("Right dist: ");
          Serial.print(distSharpRigh);
          Serial.print("  //  ");
          Serial.print("Left dist: ");
          Serial.println(distSharpLeft);
        }
}

void printRobotStatus(int movement) 
{
  if (millis() > tiempo_actual + TICK_DEBUG)
  {
    String estado_robot = "";
    if (movement == STANDBY) state = "STANDBY";
    else if (movement == SEARCH) state = "SEARCH";
    else if (movement == TURN_RIGHT) state = "TURN RIGHT";
    else if (movement == TURN_LEFT) state = "TURN LEFT";
    else if (movement == TATAMI_LIMIT) state = "TATAMI LIMIT";
    else if (movement == ATTACK) state = "ATTACK";

    Serial.print("State: ");
    Serial.println(state);
  }
}
//-------------------------------------------------------------

void setup()
{
  Serial.begin(9600);
}

void loop() 
{
  distSharpRigh = sharpRight->SharpDist(n);
  distSharpLeft = sharpLeft->SharpDist(n);
  righTatamiRead = rightTatami->TatamiRead(n);
  leftTatamiRead = LeftTatami->TatamiRead(n);

  strategy();

  if(DEBUG)
  {
    printSensors();
    printRobotStatus(strategy1);
  }
}