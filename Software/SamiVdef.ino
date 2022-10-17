//librerias
#include <Button.h>
#include <MotorSumo.h>
#include <Tatami.h>
#include <Sharp.h>
#include "BluetoothSerial.h"

//debug
#define DEBUG_SHARP 1
#define DEBUG_TATAMI 1
#define DEBUG_STATE 1
#define TICK_DEBUG 500
unsigned long currentTimeSharp = 0;
unsigned long currentTimeTatami = 0;
unsigned long currentTimeEstrategy = 0;

//configuramos el Serial Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;


//Variables y constantes para los sensores de tatami
#define PIN_SENSOR_TATAMI_IZQ 0
#define PIN_SENSOR_TATAMI_DER 1
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 300
int n = 3;

//Variables y constantes para los sensores de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO A3
#define PIN_SENSOR_DISTANCIA_IZQUIERDO A4
#define RIVAL 50
int distSharpRigh;
int distSharpLeft;

// Variables y constantes para los motores
#define PIN_MOTOR_MR1 11 //DIR
#define PIN_MOTOR_MR2PWM 10 //PWM
#define PIN_MOTOR_ML1 9 //DIR
#define PIN_MOTOR_ML2PWM 6 //PWM
#define PIN_BUTTON_START 2
#define PIN_BUTTON_STRATEGY 3  //te ponen 
#define PIN_BUZZER 5
#define SEARCH_SPEED 100
#define ATTACK_SPEED 250
#define ATTACK_SPEED_SNAKE 250
#define AVERAGE_SPEED 200;
int righSpeed = 200;
int leftSpeed = 200;
//<------------------------------------------------------------------------------------------------------------->//
//Instanciamos todos los objetos del robot
Motor *mDer = new Motor(PIN_MOTOR_MR1, PIN_MOTOR_MR2PWM, righSpeed);
Motor *mIzq = new Motor(PIN_MOTOR_ML1, PIN_MOTOR_ML2PWM, leftSpeed);

Tatami *rightTatami = new Tatami(PIN_SENSOR_TATAMI_DER);
Tatami *LeftTatami = new Tatami(PIN_SENSOR_TATAMI_IZQ);

Sharp *sharpRight = new Sharp(PIN_SENSOR_DISTANCIA_DERECHO);
Sharp *sharpLeft = new Sharp(PIN_SENSOR_DISTANCIA_IZQUIERDO);

Button *button2 = new  Button(PIN_BUTTON_STRATEGY);
Button *start = new  Button(PIN_BUTTON_START);
//<------------------------------------------------------------------------------------------------------------->//
//Funciones para indicar el lado del giro del motor y su velocidad
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

void stop()
{
  mDer->SetVelocidad(0);
  mIzq->SetVelocidad(0);
  mDer->Stop();
  mIzq->Stop();
}
//<------------------------------------------------------------------------------------------------------------->//
//Funcion para imprimir la distancia que leen los sharps en el puerto Bluetooth
void printSharp()
{
  if (millis() > currentTimeSharp + TICK_DEBUG)
  {
    currentTimeSharp = millis();
    SerialBT.print("Right dist: ");
    SerialBT.print(distSharpRigh);
    SerialBT.print("  //  ");
    SerialBT.print("Left dist: ");
    SerialBT.println(distSharpLeft);
  }
}
//Funcion para imprimir la lectura de los sensores de tatami en el puerto Bluetooth
void printTatami()
{
  if (millis() > currentTimeTatami + TICK_DEBUG)
  {
    currentTimeSharp = millis();
    SerialBT.print("Right tatami: ");
    SerialBT.print(righTatamiRead);
    SerialBT.print("  //  ");
    SerialBT.print("Left tatami: ");
    SerialBT.println(leftTatamiRead);
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Funcion para la lectura de los sensores
void sensorsReading()
  {
    distSharpRigh = sharpRight->SharpDist(n);
    distSharpLeft = sharpLeft->SharpDist(n);
    righTatamiRead = rightTatami->TatamiRead(n);
    leftTatamiRead = LeftTatami->TatamiRead(n);
  }
//<------------------------------------------------------------------------------------------------------------->//
//Con el enum reemplazamos los casos de la maquina de estado por palabras descriptivas para mejor interpretacion del codigo
enum strategy
{
  MENU,
  SNAKE,
  RONALDINHO,
  VENI_VENI,
  RIVER
};
int strategy = MENU;
int secondaryStrategy = RIVER;
//<------------------------------------------------------------------------------------------------------------->//
enum snake
{
  STANDBY_SNAKE,
  SEARCH_SNAKE,
  TURN_RIGHT_SNAKE,
  TURN_LEFT_SNAKE,
  TATAMI_LIMIT_SNAKE,
  ATTACK_SNAKE
};
int snake = STANDBY_SNAKE;
//Maquina de estados para la estrategia de serpiente
void Snake()
{
    switch (snake)
    {
    case STANDBY_SNAKE:
    {
        bool boton_start = start->GetIsPress();
        if (!boton_start) 
        {
          delay(5000);
          snake = SEARCH_SNAKE;
        } 
        else 
        {
          stop();
        }
        break;
      }

    case SEARCH_SNAKE:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) snake = TURN_RIGHT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) snake = TURN_LEFT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) snake = ATTACK_SNAKE;
    }

    case TURN_RIGHT_SNAKE:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) snake = SEARCH_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) snake = TURN_LEFT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) snake = ATTACK_SNAKE;
    }

    case TURN_LEFT_SNAKE:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        left();
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) snake = SEARCH_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) snake = TURN_RIGHT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) snake = ATTACK_SNAKE;
    }

    case ATTACK_SNAKE:
    {
        righSpeed = ATTACK_SPEED_SNAKE - (distSharpRigh);
        leftSpeed = ATTACK_SPEED_SNAKE - (distSharpLeft);
        forward();
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) snake = SEARCH_SNAKE;
        if(distSharpRigh <= RIVAL && leftTatamiRead > RIVAL) snake = TURN_RIGHT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) snake = TURN_LEFT_SNAKE;

    }

    case TATAMI_LIMIT_SNAKE: 
    {
    righSpeed = AVERAGE_SPEED;
    leftSpeed = AVERAGE_SPEED;
    backward();
    delay(300);
    snake = SEARCH_SNAKE;
    break;
    }

    }

}
//<------------------------------------------------------------------------------------------------------------->//
enum ronaldinho
{
  STANDBY_RONALDINHO,
  GO_FORWARD_RONALDINHO,
  OLEE_RONALDINHO,
  STOP_RONALDINHO,
  SWITCH_STRATEGY_RONALDINHO
};
int ronaldinho = STANDBY_RONALDINHO;
//Maquina de estados para la estrategia de Ronaldinho (porque te comes un amague de esos que hacia en sus mejores epocas)
void Ronaldinho()
{
  switch (ronaldinho)
  {
    case STANDBY_RONALDINHO:
    {
      bool boton_start = start->GetIsPress();
        if (!boton_start)
        {
          delay(5000);
          ronaldinho = GO_FORWARD_RONALDINHO;
        } 
        else 
        {
          stop();
        }
        break;
    }

    case GO_FORWARD_RONALDINHO:
    {
      righSpeed = AVERAGE_SPEED;
      leftSpeed = AVERAGE_SPEED;
      forward();
      if(leftTatamiRead < 250 || righTatamiRead < 250) ronaldinho = OLEE_RONALDINHO;
      break;
    }

    case OLEE_RONALDINHO:
    {
      righSpeed = ATTACK_SPEED;
      leftSpeed = ATTACK_SPEED;
      backward();
      delay(1000);
      ronaldinho = STOP_RONALDINHO;
    }

    case STOP_RONALDINHO:
    {
      stop();
      delay(2000);
      ronaldinho = SWITCH_STRATEGY_RONALDINHO;
    }

    case SWITCH_STRATEGY_RONALDINHO:
    {
      stop();
    }
    
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum veniVeni
{
  STANDBY_VENI_VENI,
  SEARCH_VENI_VENI,
  TURN_RIGHT_VENI_VENI,
  TURN_LEFT_VENI_VENI,
  TATAMI_LIMIT_VENI_VENI,
  ATTACK_VENI_VENI
}; 
int veniVeni = STANDBY_VENI_VENI;
//Maquina de estados para la estrategia veni veni que no te voy a hacer nada (mentira si)
void VeniVeni()
{
  switch (veniVeni)
  {
    case STANDBY_VENI_VENI:
    {
        bool boton_start = start->GetIsPress();
        if (!boton_start) 
        {
          delay(5000);
          veniVeni = SEARCH_VENI_VENI;
        } 
        else 
        {
          stop();
        }
        break;
      }

    case SEARCH_VENI_VENI:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
    }

    case TURN_RIGHT_VENI_VENI:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
    }

    case TURN_LEFT_VENI_VENI:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        left();
        if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
    }

    case ATTACK_VENI_VENI:
    {
      righSpeed = ATTACK_SPEED;
      leftSpeed = ATTACK_SPEED;
      /*if(LDR) forward();
      else 
      {
        stop();
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
      }*/
    }

    case TATAMI_LIMIT_VENI_VENI: 
    {
    righSpeed = AVERAGE_SPEED;
    leftSpeed = AVERAGE_SPEED;
    backward();
    delay(300);
    veniVeni = SEARCH_VENI_VENI;
    break;
    }

  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum river
{
  STANDBY_RIVER,
  SEARCH_RIVER,
  ATTACK_RIVER,
  TATAMI_LIMIT_RIVER
};
int river = STANDBY_RIVER;
//Maquina de estados para la estrategia del River de Gallardo (te mide y te va a buscar con todo)
void River()
{
  switch(river)
  {
    case STANDBY_RIVER:
    {
      bool boton_start = start->GetIsPress();
      if (!boton_start) 
      {
        delay(5000);
        river = SEARCH_RIVER;
      } 
      else 
      {
        stop();
      }
      break;
      }

    case SEARCH_RIVER:
    {
      righSpeed = AVERAGE_SPEED;
      leftSpeed = AVERAGE_SPEED;
      right();
      if(distSharpLeft > RIVAL) river = ATTACK_RIVER;
    }

    case ATTACK_RIVER:
    {
      righSpeed = ATTACK_SPEED;
      leftSpeed = ATTACK_SPEED;
      forward();
      if(leftTatamiRead < 250 || righTatamiRead < 250) river = TATAMI_LIMIT_RIVER;
    }

    case TATAMI_LIMIT_RIVER:
    {
      righSpeed = AVERAGE_SPEED;
      leftSpeed = AVERAGE_SPEED;
      backward;
      delay(300);
      river = SEARCH_RIVER;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Maquina de estados para el menu de la pantalla oled
void logicMovement(){
  switch (strategy)
  {
    case MENU:
    {
      //aca van todos los chiches de la pantalla oled, el objetivo es un menu prolijo, entendible y funcional.
    }
    case SNAKE:
    {
      Snake();
      break;
    }

    case RONALDINHO:
    {
      Ronaldinho();
      if(ronaldinho = SWITCH_STRATEGY_RONALDINHO) strategy = secondaryStrategy;
    }

    case VENI_VENI:
    {
      VeniVeni();
    }

    case RIVER:
    {
      River();
    }    
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Funcion para imprimir la estrategia y el caso en el puerto Bluetooth
void printStrategy() 
{
  if (millis() > currentTimeEstrategy + TICK_DEBUG)
  {
    currentTimeEstrategy = millis();
    String mode = "";
    String status = "";
    switch (strategy)
    {
      case MENU:
        mode = "MENU";
        break;
      case SNAKE:
        mode = "SNAKE";
        switch (snake)
        {
          case STANDBY_SNAKE:
          status = "STANDBY";
          break;
          case SEARCH_SNAKE:
          status = "SEARCH";
          break;
          case TURN_RIGHT_SNAKE:
          status = "TURN_RIGHT";
          break;
          case TURN_LEFT_SNAKE:
          status = "TURN_LEFT";
          break;
          case TATAMI_LIMIT_SNAKE:
          status = "TATAMI_LIMIT";
          break;
          case ATTACK_SNAKE:
          status = "ATTACK";
          break;
        }
        break;
      case RONALDINHO:
        mode = "RONALDINHO";
        switch (ronaldinho)
        {
        case STANDBY_RONALDINHO:
        status = "STANDBY";
        break;
        case GO_FORWARD_RONALDINHO:
        status = "GO_FORWARD";
        break;
        case OLEE_RONALDINHO:
        status = "OLEE";
        break;
        case STOP_RONALDINHO:
        status = "STOP";
        break;
        case SWITCH_STRATEGY_RONALDINHO:
        status = "SWITCH_STRATEGY";
        break;
        }
        break;
      case VENI_VENI:
        mode = "VENI_VENI";
        switch (veniVeni)
        {
        case STANDBY_VENI_VENI:
        status = "STANDBY";
        break;
        case SEARCH_VENI_VENI:
        status = "SEARCH";
        break;
        case TURN_RIGHT_VENI_VENI:
        status = "TURN_RIGHT";
        break;
        case TURN_LEFT_VENI_VENI:
        status = "TURN_LEFT";
        break;
        case TATAMI_LIMIT_VENI_VENI:
        status = "TATAMI_LIMIT";
        break;
        case ATTACK_VENI_VENI:
        status = "ATTACK";
        break;
        }
        break;
      case RIVER:
        mode = "RIVER";
        switch (river)
        {
          case STANDBY_RIVER:
          status = "STANDBY";
          break;
          case SEARCH_RIVER:
          status = "SEARCH";
          break;
          case ATTACK_RIVER:
          status = "ATTACK";
          break;
          case TATAMI_LIMIT_RIVER:
          status = "TATAMI";
          break;
        }
        break;
      }
    SerialBT.print("Strategy: ");
    SerialBT.print(mode);
    SerialBT.print("  //  Case: ");
    SerialBT.println(status);
  }
}
//<------------------------------------------------------------------------------------------------------------->//

void setup()
{
  SerialBT.begin("Bover");
  Serial.begin(9600);
}

void loop() 
{ 
  sensorsReading();
  logicMovement();
  if(DEBUG_SHARP) printSharp();
  if(DEBUG_TATAMI) printTatami();
  if(DEBUG_STATE) printStrategy();
}