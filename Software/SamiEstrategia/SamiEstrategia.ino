#include <Button.h>
#include <MotorSumo.h>
#include <Tatami.h>
#include <Sharp.h>

#define DEBUG 0
#define TICK_DEBUG 500
unsigned long tiempo_actual = 0;


//Sensors de tatami
#define PIN_SENSOR_TATAMI_IZQ A0
#define PIN_SENSOR_TATAMI_DER A1
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 250
int n = 3;
#define DELAY_BACK 200

//sensor de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO A6
#define PIN_SENSOR_DISTANCIA_IZQUIERDO A7
#define RIVAL 55
int distSharpRigh;
int distSharpLeft;

// motor
#define PIN_MOTOR_MR1 11 //DIR
#define PIN_MOTOR_MR2PWM 10 //PWM
#define PIN_MOTOR_ML1 9 //DIR
#define PIN_MOTOR_ML2PWM 6 //PWM
#define PIN_BUTTON_START 3
bool boton_start;
#define PIN_BUTTON_STRATEGY 2  //te ponen 
#define PIN_BUZZER 5
#define SEARCH_SPEED 100
#define ATTACK_SPEED 200
#define AVERAGE_SPEED 250;
int righSpeed = 150;
int leftSpeed = 150;
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

enum strategy{
  STANDBY,
  SEARCH,
  TURN_RIGHT,
  TURN_LEFT,
  TATAMI_LIMIT,
  ATTACK
};
int strategy1 = STANDBY;

void strategya()
{
    switch (strategy1)
    {
    case STANDBY:
    {
        boton_start = start->GetIsPress();
        if (boton_start) 
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
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) strategy1 = TURN_RIGHT;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) strategy1 = TURN_LEFT;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) strategy1 = ATTACK;
    break;
    }

    case TURN_RIGHT:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) strategy1 = SEARCH;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) strategy1 = TURN_LEFT;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) strategy1 = ATTACK;
    break;
    }

    case TURN_LEFT:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        left();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) strategy1 = SEARCH;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) strategy1 = TURN_RIGHT;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) strategy1 = ATTACK;
    break;
    }

    case ATTACK:
    {
        righSpeed = ATTACK_SPEED; //+ (distSharpRigh * (-2));
        leftSpeed = ATTACK_SPEED; //+ (distSharpLeft * (-2));
        forward();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) strategy1 = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) strategy1 = SEARCH;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) strategy1 = TURN_RIGHT;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) strategy1 = TURN_LEFT;
    break;
    }

    case TATAMI_LIMIT: 
    {
    righSpeed = AVERAGE_SPEED;
    leftSpeed = AVERAGE_SPEED;
    backward();
    delay(DELAY_BACK);
    if(leftTatamiRead > BORDE_TATAMI && righTatamiRead > BORDE_TATAMI) strategy1 = SEARCH;
    
    break;
    }

    }

}
//-------------------------------------------------------------

void printSensors()
{
  if (millis() > tiempo_actual + TICK_DEBUG)
        {
          Serial.print("  Right tatami: ");
          Serial.print(righTatamiRead);
          Serial.print("  //  ");
          Serial.print("  Left tatami: ");
          Serial.print(leftTatamiRead);
          Serial.print("  Right dist: ");
          Serial.print(distSharpRigh);
          Serial.print("  //  ");
          Serial.print("  Left dist: ");
          Serial.println(distSharpLeft);
        }
}

void printRobotStatus(int movement) 
{
  if (millis() > tiempo_actual + TICK_DEBUG)
  {
    String state = "";
    if (movement == STANDBY) state = " STANDBY";
    else if (movement == SEARCH) state = " SEARCH";
    else if (movement == TURN_RIGHT) state = " TURN RIGHT";
    else if (movement == TURN_LEFT) state = " TURN LEFT";
    else if (movement == TATAMI_LIMIT) state = " TATAMI LIMIT";
    else if (movement == ATTACK) state = " ATTACK";

    Serial.print("State: ");
    Serial.println(state);
    Serial.print(" || ");
    Serial.println( boton_start);
  }
}
//-------------------------------------------------------------

void setup()
{
  Serial.begin(9600);
  /*forward();
  delay(5000);
  backward();
  delay(5000);
  left();
  delay(3000);
  right();
  delay(3000);
  stopMotor();
  delay(3000);
  */
}

void loop() 
{
  distSharpRigh = sharpRight->SharpDist(n);
  distSharpLeft = sharpLeft->SharpDist(n);
  righTatamiRead = rightTatami->TatamiRead(n);
  leftTatamiRead = LeftTatami->TatamiRead(n);

  strategya();

  if(DEBUG)
  {
    printSensors();
    printRobotStatus(strategy1);
  }
}
