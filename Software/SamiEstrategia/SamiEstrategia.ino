#include <Button.h>
#include <MotorSumo.h>
#include <Tatami.h>
#include <Sharp.h>

#define DEBUG_MODES 0
#define DEBUG_SENSORES 0
#define TICK_DEBUG 500
unsigned long tiempo_actual = 0;


//Sensors de tatami
#define PIN_SENSOR_TATAMI_IZQ 26
#define PIN_SENSOR_TATAMI_DER 27
int rightTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 250
int n = 3;
#define DELAY_BACK 70

//sensor de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO 32
#define PIN_SENSOR_DISTANCIA_IZQUIERDO 33
int distSharpRight;
int distSharpLeft;
#define RIVAL 30

// motor
#define PIN_MOTOR_MR1 22 //DIR
#define PIN_MOTOR_MR2PWM 21 //PWM
#define PIN_MOTOR_ML1 19 //DIR
#define PIN_MOTOR_ML2PWM 18 //PWM
//velocidades
#define SEARCH_SPEED 30
#define ATTACK_SPEED 120
#define AVERAGE_SPEED 120;
int rightSpeed = 150;
int leftSpeed = 150;
//buttons
#define PIN_BUTTON_START 34
bool boton_start;
#define PIN_BUTTON_STRATEGY 35
//buzzer
#define PIN_BUZZER 23

//-------------------------------------------------------------

Motor *mDer = new Motor(PIN_MOTOR_MR1, PIN_MOTOR_MR2PWM, rightSpeed);
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
  mDer->SetVelocidad(rightSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Forward();
  mIzq->Forward();
}

void backward()
{
  mDer->SetVelocidad(rightSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Backward();
  mIzq->Backward();
}
 
void left()
{
  mDer->SetVelocidad(rightSpeed);
  mIzq->SetVelocidad(leftSpeed);
  mDer->Forward();
  mIzq->Backward();
}


void right()
{
  mDer->SetVelocidad(rightSpeed);
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
        mode = SEARCH;
        
        break;
        }
     
    
      

    case SEARCH:
    {
        rightSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < BORDE_TATAMI || rightTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRight <= RIVAL && distSharpLeft > RIVAL) mode = TURN_RIGHT;
        if(distSharpRight > RIVAL && distSharpLeft <= RIVAL) mode = TURN_LEFT;
        if(distSharpRight <= RIVAL && distSharpLeft <= RIVAL) mode = ATTACK;
    break;
    }

    case TURN_RIGHT:
    {
        rightSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < BORDE_TATAMI || rightTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRight > RIVAL && distSharpLeft > RIVAL) mode = SEARCH;
        if(distSharpRight > RIVAL && distSharpLeft <= RIVAL) mode = TURN_LEFT;
        if(distSharpRight <= RIVAL && distSharpLeft <= RIVAL) mode = ATTACK;
    break;
    }

    case TURN_LEFT:
    {
        rightSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        left();
        if(leftTatamiRead < BORDE_TATAMI || rightTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRight > RIVAL && distSharpLeft > RIVAL) mode = SEARCH;
        if(distSharpRight <= RIVAL && distSharpLeft > RIVAL) mode = TURN_RIGHT;
        if(distSharpRight <= RIVAL && distSharpLeft <= RIVAL) mode = ATTACK;
    break;
    }

    case ATTACK:
    {
        rightSpeed = ATTACK_SPEED; //+ (distSharpRight * (-2));
        leftSpeed = ATTACK_SPEED; //+ (distSharpLeft * (-2));
        forward();
        if(leftTatamiRead < BORDE_TATAMI || righTatamiRead < BORDE_TATAMI) mode = TATAMI_LIMIT;
        if(distSharpRight > RIVAL && distSharpLeft > RIVAL) mode = SEARCH;
        if(distSharpRight <= RIVAL && distSharpLeft > RIVAL) mode = TURN_RIGHT;
        if(distSharpRight > RIVAL && distSharpLeft <= RIVAL) mode = TURN_LEFT;
    break;
    }

    case TATAMI_LIMIT: 
    {
    rightSpeed = AVERAGE_SPEED;
    leftSpeed = AVERAGE_SPEED;
    backward();
    delay(DELAY_BACK);
    if(leftTatamiRead > BORDE_TATAMI && rightTatamiRead > BORDE_TATAMI) mode = SEARCH;
    
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
          Serial.print(rightTatamiRead);
          Serial.print("  //  ");
          Serial.print("  Left tatami: ");
          Serial.print(leftTatamiRead);
          Serial.print("  Right dist: ");
          Serial.print(distSharpRight);
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
  forward();
  delay(5000);
  backward();
  delay(5000);
  left();
  delay(3000);
  right();
  delay(3000);
  stopMotor();
  delay(3000);
  
}

void loop() 
{
  distSharpRight = sharpRight->SharpDist(n);
  distSharpLeft = sharpLeft->SharpDist(n);
  rightTatamiRead = rightTatami->TatamiRead(n);
  leftTatamiRead = LeftTatami->TatamiRead(n);

  estrategia();

  if(DEBUG_SENSORES)
   {   
    printSensors();
  }
  if(DEBUG_MODES)
  { 
    printRobotStatus(mode);
  }
}
