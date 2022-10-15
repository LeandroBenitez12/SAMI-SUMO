#include <Button.h>
#include <MotorSumo.h>
#include <Tatami.h>
#include <Sharp.h>

#define DEBUG_MODOS 0
#define DEBUG_SENSORES 0
#define TICK_DEBUG 500
unsigned long tiempo_actual = 0;


//Sensors de tatami
#define PIN_SENSOR_TATAMI_IZQ 26
#define PIN_SENSOR_TATAMI_DER 27
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 250
int n = 3;
#define DELAY_BACK 100

//sensor de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO 25
#define PIN_SENSOR_DISTANCIA_IZQUIERDO 33
#define RIVAL 30
int distSharpRigh;
int distSharpLeft;

// motor
#define PIN_MOTOR_MR1 21 //DIR
#define PIN_MOTOR_MR2PWM 22 //PWM
#define PIN_MOTOR_ML1 19 //DIR
#define PIN_MOTOR_ML2PWM 18 //PWM
#define PIN_BUTTON_START 34
#define PIN_BUTTON_STRATEGY 35 //te ponen 
#define PIN_BUZZER 23
bool boton_start;
bool boton_strategy;
#define SEARCH_SPEED 50
#define ATTACK_SPEED 255
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

enum strategys{
  STANDBY,
  SEARCH,
  TURN_RIGHT,
  TURN_LEFT,
  TATAMI_LIMIT,
  ATTACK
};
int mode = STANDBY;

void estrategia()
{
    switch (mode)
    {
    case STANDBY:
    {
      boton_start = start->getIsPress();
      boton_strategy = strategy->getIsPress();

      if (boton_start){
        mode = SEARCH;
      }
      else stopMotor();
        
        
        break;
        }

    case SEARCH:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) mode = TURN_RIGHT;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) mode = TURN_LEFT;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) mode = ATTACK;
    break;
    }
  
    case TURN_RIGHT:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) mode = SEARCH;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) mode = TURN_LEFT;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) mode = ATTACK;
    break;
    }

    case TURN_LEFT:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        left();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) mode = SEARCH;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) mode = TURN_RIGHT;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) mode = ATTACK;
    break;
    }

    case ATTACK:
    {
        righSpeed = ATTACK_SPEED; //+ (distSharpRigh * (-2));
        leftSpeed = ATTACK_SPEED; //+ (distSharpLeft * (-2));
        forward();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) mode = SEARCH;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) mode = TURN_RIGHT;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) mode = TURN_LEFT;
    break;
    }

    case TATAMI_LIMIT: 
    {
    righSpeed = AVERAGE_SPEED;
    leftSpeed = AVERAGE_SPEED;
    backward();
    delay(DELAY_BACK);
    if(leftTatamiRead > BORDE_TATAMI && righTatamiRead > BORDE_TATAMI) mode = SEARCH;
    
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
  delay(5000);
  //while(true)Serial.println(digitalRead(3));delay(500);
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

  estrategia();

  if(DEBUG_SENSORES)
  {
    printSensors();
  }
  if(DEBUG_MODOS)
  {
    printRobotStatus(mode);
  }
  
}
