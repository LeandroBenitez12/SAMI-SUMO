//librerias
#include <ButtonRyo.h>
#include <Tatami.h>
#include <Sharp.h>
#include "BluetoothSerial.h"

//debug
#define DEBUG_SHARP 0
#define DEBUG_TATAMI 0
#define DEBUG_STATE 0
#define DEBUG_LDR 0
#define TICK_DEBUG 500
#define TICK_DEBUG_STRATEGY 500
#define TICK_DEBUG_SHARP 500
#define TICK_DEBUG_TATAMI 500
#define TICK_DEBUG_LDR 1000
unsigned long currentTimeSharp = 0;
unsigned long currentTimeTatami = 0;
unsigned long currentTimeEstrategy = 0;
unsigned long currentTimeLdr = 0;

//configuramos el Serial Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial SerialBT;

//Variables y constantes para los sensores de tatami
#define PIN_SENSOR_TATAMI_IZQ 13
#define PIN_SENSOR_TATAMI_DER 27
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 300

//Variables y constantes para los sensores de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO 32
#define PIN_SENSOR_DISTANCIA_IZQUIERDO 33
#define RIVAL 43
int distSharpRigh;
int distSharpLeft;

// Variables y constantes para los motores
#define PIN_ENGINE_DIR_LEFT 22 //DIR
#define PIN_ENGINE_PWM_LEFT 21 //PWM
#define PIN_ENGINE_DIR_RIGHT 19 //DIR
#define PIN_ENGINE_PWM_RIGHT 18 //PWM
#define PWM_CHANNEL_RIGHT 12
#define PWM_CHANNEL_LEFT 11
#define SEARCH_SPEED 75// 12 volt 170
#define ATTACK_SPEED_LDR 255// 12 volt 255
#define ATTACK_SPEED 180// 12 volt 220
#define STRONG_ATTACK_SPEED 210
#define ATTACK_SPEED_AGGRESSIVE 240// 12 volt 235
#define AVERAGE_SPEED 100// 12 volt 200
int slowAttack = 40; // 12 volt 120
int lowAttackCont;
unsigned long currentTimeAttack = 0;
int tickTurn;
#define TICK_LOW_ATTACK 1600
#define TICK_ATTACK_SEARCH 1500
#define TICK_TURN_FRONT 59// 12 volt 115 45°
#define TICK_TURN_SIDE 95// 12 volt 168 90°
#define TICK_SHORT_BACK_TURN 115// 12 volt 200 110°
#define TICK_LONG_BACK_TURN 120// 12 volt 135°


//Variables y constantes LDR
#define PIN_SENSOR_LDR 25
#define MONTADO 100
int ldr;

//Pines para los botones y buzzer
#define PIN_BUTTON_START 18
enum
{
    NONE,
    SWITCH,
    START
};
#define BUZZER 16
//<------------------------------------------------------------------------------------------------------------->//

Tatami *rightTatami = new Tatami(PIN_SENSOR_TATAMI_DER);
Tatami *LeftTatami = new Tatami(PIN_SENSOR_TATAMI_IZQ);

Sharp *sharpRight = new Sharp(PIN_SENSOR_DISTANCIA_DERECHO);
Sharp *sharpLeft = new Sharp(PIN_SENSOR_DISTANCIA_IZQUIERDO);

Button *start = new  Button(PIN_BUTTON_START);
//<------------------------------------------------------------------------------------------------------------->//
//Sensor LDR
int LdrRead(int p){
  int lectura(analogRead(p));
  return lectura;
}
//<------------------------------------------------------------------------------------------------------------->//
//Funcion para imprimir la distancia que leen los sharps en el puerto Bluetooth
void printLdr()
{
  if (millis() > currentTimeLdr + TICK_DEBUG_LDR)
  {
    currentTimeLdr = millis();
    SerialBT.print("Ldr: ");
    SerialBT.println(ldr);
  }
}
void printSharp()
{
  if (millis() > currentTimeSharp + TICK_DEBUG_SHARP)
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
  if (millis() > currentTimeTatami + TICK_DEBUG_TATAMI)
  {
    currentTimeTatami = millis();
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
    distSharpRigh = sharpRight->SharpDist();
    distSharpLeft = sharpLeft->SharpDist();
    righTatamiRead = rightTatami->TatamiRead();
    leftTatamiRead = LeftTatami->TatamiRead();
  }
//<------------------------------------------------------------------------------------------------------------->//
//Con el enum reemplazamos los casos de la maquina de estado por palabras descriptivas para mejor interpretacion del codigo
enum strategy
{
  REPOSITIONING_MENU,
  STRATEGIES_MENU,
  PASSIVE,
  SEMI_PASSIVE,
  SEMI_AGGRESSIVE,
  AGGRESSIVE
};
int strategy = REPOSITIONING_MENU;
//<------------------------------------------------------------------------------------------------------------->//
enum passive
{
  STANDBY_PASSIVE,
  SEARCH_PASSIVE,
  TURN_RIGHT_PASSIVE,
  TURN_LEFT_PASSIVE,
  TATAMI_LIMIT_PASSIVE,
  ATTACK_PASSIVE
}; 
int passive = STANDBY_PASSIVE;
//Estrategia que espera al rival y cuando este se monta ataca
void Passive()
{
  switch (passive)
  {
    case STANDBY_PASSIVE:
    {
    Ryo->Stop();
    if (start->GetIsPress())
    {
      delay(5000);
      Ryo->Right(ATTACK_SPEED, ATTACK_SPEED);
      delay(tickTurn);
      passive = SEARCH_PASSIVE;
    } 
    break;
    }

    case SEARCH_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) passive = TURN_RIGHT_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) passive = TURN_LEFT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) passive = ATTACK_PASSIVE;
      break;    
    }

    case TURN_RIGHT_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) passive = SEARCH_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) passive = TURN_LEFT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) passive = ATTACK_PASSIVE;
      break;
    }

    case TURN_LEFT_PASSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) passive = SEARCH_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) passive = TURN_RIGHT_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) passive = ATTACK_PASSIVE;
      break;
    }

    case ATTACK_PASSIVE:
    {
      if(ldr < MONTADO)
      {
        Ryo->Forward(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
        if(leftTatamiRead < 250 || righTatamiRead < 250) passive = TATAMI_LIMIT_PASSIVE;
      }

      else 
      {
        Ryo->Stop();
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) passive = SEARCH_PASSIVE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) passive = TURN_RIGHT_PASSIVE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) passive = TURN_LEFT_PASSIVE;
      }
      break;
    }

    case TATAMI_LIMIT_PASSIVE: 
    {
    Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
    delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) passive = SEARCH_PASSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum semiPassive
{
  STANDBY_SEMI_PASSIVE,
  SEARCH_SEMI_PASSIVE,
  TURN_RIGHT_SEMI_PASSIVE,
  TURN_LEFT_SEMI_PASSIVE,
  LOW_ATTACK_SEMI_PASSIVE,
  TATAMI_LIMIT_SEMI_PASSIVE,
  ATTACK_SEMI_PASSIVE
}; 
int semiPassive = STANDBY_SEMI_PASSIVE;
//Estrategia que espera al rival y avanza de a poco buscando el rival
void SemiPassive()
{
  switch (semiPassive)
  {
    case STANDBY_SEMI_PASSIVE:
    {
    Ryo->Stop();
    if (start->GetIsPress())
    {
      delay(5000);
      Ryo->Right(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
      delay(tickTurn);
      semiPassive = SEARCH_SEMI_PASSIVE;
    } 
    break;
    }

    case SEARCH_SEMI_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiPassive = TURN_RIGHT_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiPassive = TURN_LEFT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiPassive = ATTACK_SEMI_PASSIVE;
      if (millis() > currentTimeAttack + TICK_ATTACK_SEARCH)
      {
        semiPassive = LOW_ATTACK_SEMI_PASSIVE;
      }
      if(ldr < MONTADO)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }
      break;    
    }

    case TURN_RIGHT_PASSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiPassive = SEARCH_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiPassive = TURN_LEFT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiPassive = ATTACK_SEMI_PASSIVE;
      if(ldr < MONTADO)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }
      break;
    }

    case TURN_LEFT_PASSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiPassive = SEARCH_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiPassive = TURN_RIGHT_SEMI_PASSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiPassive = ATTACK_SEMI_PASSIVE;
      if(ldr < MONTADO)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }
      break;
    }

    case ATTACK_SEMI_PASSIVE:
    {
      if(ldr < MONTADO)
      {
        Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_PASSIVE;
      }

      else 
      {
        Ryo->Stop();
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiPassive = SEARCH_SEMI_PASSIVE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiPassive = TURN_RIGHT_SEMI_PASSIVE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiPassive = TURN_LEFT_SEMI_PASSIVE;
        if (millis() > currentTimeAttack + TICK_LOW_ATTACK)
        {
          semiPassive = LOW_ATTACK_SEMI_PASSIVE;
        }
      }
      break;
    }

    case LOW_ATTACK_SEMI_PASSIVE:
    {
      lowAttackCont++;
      slowAttack = slowAttack + (lowAttackCont*10);
      Ryo->Forward(slowAttack, slowAttack);
      delay(388);
      currentTimeAttack = millis();
      semiPassive = ATTACK_SEMI_PASSIVE;
      break;
    }

    case TATAMI_LIMIT_SEMI_PASSIVE: 
    {
        Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
        delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) semiPassive = SEARCH_SEMI_PASSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum semiAggressive
{
  STANDBY_SEMI_AGGRESSIVE,
  SEARCH_SEMI_AGGRESSIVE,
  TURN_RIGHT_SEMI_AGGRESSIVE,
  TURN_LEFT_SEMI_AGGRESSIVE,
  TATAMI_LIMIT_SEMI_AGGRESSIVE,
  ATTACK_SEMI_AGGRESSIVE
}; 
int semiAggressive = STANDBY_SEMI_AGGRESSIVE;
//Estrategia que va a buscar al rival de forma moderada
void SemiAggressive()
{
  switch (semiAggressive)
  {
    case STANDBY_SEMI_AGGRESSIVE:
    {
    Ryo->Stop();
    if (start->GetIsPress())
    {
      delay(5000);
      Ryo->Right(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
      delay(tickTurn);
      semiAggressive = SEARCH_SEMI_AGGRESSIVE;
    } 
    break;
    }

    case SEARCH_SEMI_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiAggressive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiAggressive = TURN_RIGHT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiAggressive = TURN_LEFT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiAggressive = ATTACK_SEMI_AGGRESSIVE; 
    }

    case TURN_RIGHT_SEMI_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiAggressive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiAggressive = TURN_LEFT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiAggressive = ATTACK_SEMI_AGGRESSIVE;
      break;
    }

    case TURN_LEFT_SEMI_AGGRESSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiAggressive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiAggressive = TURN_RIGHT_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) semiAggressive = ATTACK_SEMI_AGGRESSIVE;
      break;
    }

    case ATTACK_SEMI_AGGRESSIVE:
    {
      Ryo->Forward(ATTACK_SPEED, ATTACK_SPEED);
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) semiAggressive = TURN_RIGHT_SEMI_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) semiAggressive = TURN_LEFT_SEMI_AGGRESSIVE;
      if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      if(ldr < MONTADO) 
      {
        Ryo->Forward(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
        if(leftTatamiRead < 250 || righTatamiRead < 250) semiPassive = TATAMI_LIMIT_SEMI_AGGRESSIVE;
      }
      if(distSharpRigh > 15 && distSharpLeft > 15)
      {
        Ryo->Forward(STRONG_ATTACK_SPEED, STRONG_ATTACK_SPEED);
      }
      break;
    }

    case TATAMI_LIMIT_SEMI_AGGRESSIVE: 
    {
      Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
      delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) semiAggressive = SEARCH_SEMI_AGGRESSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum aggressive
{
  STANDBY_AGGRESSIVE,
  SEARCH_AGGRESSIVE,
  TURN_RIGHT_AGGRESSIVE,
  TURN_LEFT_AGGRESSIVE,
  TATAMI_LIMIT_AGGRESSIVE,
  ATTACK_AGGRESSIVE
}; 
int aggressive = STANDBY_AGGRESSIVE;
//Estrategia que va a buscar al rival de forma moderada
void Aggressive()
{
  switch (aggressive)
  {
    case STANDBY_AGGRESSIVE:
    {
    Ryo->Stop();
    if (start->GetIsPress())
    {
      delay(5000);
      Ryo->Right(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
      delay(tickTurn);
      aggressive = SEARCH_AGGRESSIVE;
    } 
    break;
    }

    case SEARCH_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) aggressive = TURN_RIGHT_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) aggressive = TURN_LEFT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) aggressive = ATTACK_AGGRESSIVE; 
    }

    case TURN_RIGHT_AGGRESSIVE:
    {
      Ryo->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) aggressive = SEARCH_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) aggressive = TURN_LEFT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) aggressive = ATTACK_AGGRESSIVE;
      break;
    }

    TURN_LEFT_AGGRESSIVE:
    {
      Ryo->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) aggressive = SEARCH_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) aggressive = TURN_RIGHT_AGGRESSIVE;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) aggressive = ATTACK_AGGRESSIVE;
      break;
    }


    case ATTACK_AGGRESSIVE:
    {
        Ryo->Forward(ATTACK_SPEED_AGGRESSIVE, ATTACK_SPEED_AGGRESSIVE);
        if(distSharpRigh > RIVAL || distSharpLeft > RIVAL) aggressive = SEARCH_AGGRESSIVE;
        if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
        if(ldr < MONTADO) 
        {
          Ryo->Forward(ATTACK_SPEED_LDR, ATTACK_SPEED_LDR);
          if(leftTatamiRead < 250 || righTatamiRead < 250) aggressive = TATAMI_LIMIT_AGGRESSIVE;
        }
        break;
    }

    case TATAMI_LIMIT_AGGRESSIVE: 
    {
    Ryo->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
    delay(300);
    if(leftTatamiRead > 250 && righTatamiRead > 250) aggressive = SEARCH_AGGRESSIVE;
    break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Maquina de estados para el menu de la pantalla oled
//Maquina de estados para seleccionar el angulo de giro de reposicionamiento
enum repositioningMenu
{
  TURN_MAIN_MENU,
  TURN_FRONT,
  TURN_SIDE,
  SHORT_BACK_TURN,
  LONG_BACK_TURN
};
int repositioningMenu = TURN_MAIN_MENU;

void RepositioningMenu()
{
  switch (repositioningMenu)
  {
  case TURN_MAIN_MENU:
  {
    if(start->GetIsPress() == SWITCH) repositioningMenu = TURN_FRONT;
    if(start->GetIsPress()) 
    {
      tickTurn = 0;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case TURN_FRONT:
  {
    if(start->GetIsPress() == SWITCH) repositioningMenu = TURN_SIDE;
    if(start->GetIsPress() == START)
    {
      tickTurn = TICK_TURN_FRONT;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case TURN_SIDE:
  {
    if(start->GetIsPress() == SWITCH) repositioningMenu = SHORT_BACK_TURN;
    if(start->GetIsPress() == START)
    {
      tickTurn = TICK_TURN_SIDE;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case SHORT_BACK_TURN:
  {
    if(start->GetIsPress() == SWITCH) repositioningMenu = LONG_BACK_TURN;
    if(start->GetIsPress() == START)
    {
      tickTurn = TICK_SHORT_BACK_TURN;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case LONG_BACK_TURN:
  {
    if(start->GetIsPress() == SWITCH) repositioningMenu = TURN_FRONT;
    if(start->GetIsPress() == START)
    {
      tickTurn = TICK_LONG_BACK_TURN;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  }
}

enum strategiesMenu
{
  MAIN_MENU,
  PASSIVE_MENU,
  SEMI_PASSIVE_MENU,
  SEMI_AGGRESSIVE_MENU,
  AGGRESSIVE_MENU,
};
int menu = MAIN_MENU;
//Maquina de estados para navegar dentro del menu y seleccionar la estrategia
void StrategiesMenu()
{
  switch (menu)
  {
  case MAIN_MENU:
  {
    if(start->GetIsPress() == SWITCH) menu = PASSIVE_MENU;
    break;
  }
  
  case PASSIVE_MENU:
  { 
    if(start->GetIsPress() == SWITCH) menu = SEMI_PASSIVE_MENU;
    if(start->GetIsPress() == START) strategy = PASSIVE;
    break;
  }

  case SEMI_PASSIVE_MENU:
  {  
    if(start->GetIsPress() == SWITCH) menu = SEMI_AGGRESSIVE_MENU;
    if(start->GetIsPress() == START) strategy = SEMI_PASSIVE;
    break;
  }

  case SEMI_AGGRESSIVE_MENU:
  { 
    if(start->GetIsPress() == SWITCH) menu = AGGRESSIVE_MENU;
    if(start->GetIsPress() == START) strategy = SEMI_AGGRESSIVE;
    break;
  }

  case AGGRESSIVE_MENU:
  {
    if(start->GetIsPress() == SWITCH) menu = PASSIVE_MENU;
    if(start->GetIsPress() == d) menu = WAIT_FOR_ATTACK;
    break;
  }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
void logicMovement()
{
  switch (strategy)
  {
    case REPOSITIONING_MENU:
    {
      RepositioningMenu();
      break;
    }
    case STRATEGIES_MENU:
    {
      StrategiesMenu();
      break;
    }
    case PASSIVE:
    {
      Passive();
      break;
    }

    case SEMI_PASSIVE:
    {
      SemiPassive();
      break;
    }

    case SEMI_AGGRESSIVE:
    {
      SemiAggressive();
      break;
    }

    case AGGRESSIVE:
    {
      Aggressive();
      break;
    }   
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Funcion para imprimir la estrategia y el caso en el puerto Bluetooth
/*void printStrategy() 
{
  if (millis() > currentTimeEstrategy + TICK_DEBUG_STRATEGY)
  {
    currentTimeEstrategy = millis();
    String mode = "";
    String status = "";
    switch (strategy)
    {
      case REPOSITIONING_MENU:
        mode = "REPOSITIONING_MENU";
        break;
      case STRATEGIES_MENU:
        mode = "STRATEGIES_MENU";
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
}*/
//<------------------------------------------------------------------------------------------------------------->//

void setup()
{
  SerialBT.begin("Sami");
  Serial.begin(9600);
  display.init();
}

void loop(){ 
  sensorsReading();
  ldr = LdrRead(PIN_SENSOR_LDR);
  logicMovement();
  if(DEBUG_SHARP) printSharp();
  if(DEBUG_TATAMI) printTatami();
  //if(DEBUG_STATE) printStrategy();
  if(DEBUG_LDR) printLdr();
}
