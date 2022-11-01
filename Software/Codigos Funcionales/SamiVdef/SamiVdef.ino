//librerias
#include <Button.h>
#include <SumoEngineController.h>
#include <Tatami.h>
#include <Sharp.h>
#include "BluetoothSerial.h"
#include <SSD1306.h>

//debug
#define DEBUG_SHARP 1
#define DEBUG_TATAMI 1
#define DEBUG_STATE 1
#define DEBUG_LDR 1
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
#define PIN_SENSOR_TATAMI_IZQ 33
#define PIN_SENSOR_TATAMI_DER 25
int righTatamiRead = 1000;
int leftTatamiRead = 1000;
#define BORDE_TATAMI 300

//Variables y constantes para los sensores de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO 27
#define PIN_SENSOR_DISTANCIA_IZQUIERDO 26
#define RIVAL 30
int distSharpRigh;
int distSharpLeft;

// Variables y constantes para los motores
#define PIN_ENGINE_DIR_LEFT 22 //DIR
#define PIN_ENGINE_PWM_LEFT 21 //PWM
#define PIN_ENGINE_DIR_RIGHT 19 //DIR
#define PIN_ENGINE_PWM_RIGHT 18 //PWM
#define SEARCH_SPEED 200
#define ATTACK_SPEED 255
#define ATTACK_SPEED_SNAKE 255
#define AVERAGE_SPEED 200
int tickTurn;
#define TICK_TURN_FRONT 115
#define TICK_TURN_SIDE 168
#define TICK_SHORT_BACK_TURN 200
#define TICK_LONG_BACK_TURN 235


//Variables y constantes LDR
#define PIN_SENSOR_LDR 32
#define MONTADO 100
int ldr;

//Pines para los botones
#define PIN_BUTTON_START 34
#define PIN_BUTTON_STRATEGY 35

// variables y constantes para la pantalla oled
#define PIN_SDA 16
#define PIN_SCL 17
//<------------------------------------------------------------------------------------------------------------->//
//Instanciamos todos los objetos del robot
SSD1306 display (0x3C,PIN_SDA, PIN_SCL); // inicializa pantalla con direccion 0x3C

EngineController *Sami = new EngineController(PIN_ENGINE_DIR_RIGHT, PIN_ENGINE_PWM_RIGHT, PIN_ENGINE_DIR_LEFT, PIN_ENGINE_PWM_LEFT);

Tatami *rightTatami = new Tatami(PIN_SENSOR_TATAMI_DER);
Tatami *LeftTatami = new Tatami(PIN_SENSOR_TATAMI_IZQ);

Sharp *sharpRight = new Sharp(PIN_SENSOR_DISTANCIA_DERECHO);
Sharp *sharpLeft = new Sharp(PIN_SENSOR_DISTANCIA_IZQUIERDO);

Button *selectStrategy = new  Button(PIN_BUTTON_STRATEGY);
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
    //righTatamiRead = rightTatami->TatamiRead();
    //leftTatamiRead = LeftTatami->TatamiRead();
  }
//<------------------------------------------------------------------------------------------------------------->//
//Con el enum reemplazamos los casos de la maquina de estado por palabras descriptivas para mejor interpretacion del codigo
enum strategy
{
  REPOSITIONING_MENU,
  STRATEGIES_MENU,
  SNAKE,
  RONALDINHO,
  VENI_VENI,
  RIVER,
  SAN_LORENZO
};
int strategy = REPOSITIONING_MENU;
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
      display.clear();   
      display.drawString(19, 0, "Strategy Snake"); 
      display.drawString(0, 9, "---------------------"); 
      display.drawString(0,28, "Press Star()");    
      display.display();
      if (start->GetIsPress()) 
      {
        display.clear();   
        display.drawString(19, 0, "Strategy Snake"); 
        display.drawString(0, 9, "---------------------"); 
        display.drawString(0,28, "Iniciando en 5");
        display.display();
        delay(5000);
        Sami->Right(ATTACK_SPEED, ATTACK_SPEED);
        delay(tickTurn);
        snake = SEARCH_SNAKE;
      } 
      else Sami->Stop();
      break;
    }

    case SEARCH_SNAKE:
    {
        Sami->Right(ATTACK_SPEED, ATTACK_SPEED);
        delay(tickTurn);
        Sami->Right(SEARCH_SPEED, SEARCH_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) snake = TURN_RIGHT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) snake = TURN_LEFT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) snake = ATTACK_SNAKE;
        break;
    }

    case TURN_RIGHT_SNAKE:
    {
        Sami->Right(SEARCH_SPEED, SEARCH_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) snake = SEARCH_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) snake = TURN_LEFT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) snake = ATTACK_SNAKE;
        break;
    }

    case TURN_LEFT_SNAKE:
    {
        Sami->Left(SEARCH_SPEED, SEARCH_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) snake = SEARCH_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) snake = TURN_RIGHT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) snake = ATTACK_SNAKE;
        break;
    }

    case ATTACK_SNAKE:
    {
        int righSpeed = ATTACK_SPEED_SNAKE - (distSharpRigh);
        int leftSpeed = ATTACK_SPEED_SNAKE - (distSharpLeft);
        Sami->Right(righSpeed, leftSpeed);
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) snake = SEARCH_SNAKE;
        if(distSharpRigh <= RIVAL && leftTatamiRead > RIVAL) snake = TURN_RIGHT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) snake = TURN_LEFT_SNAKE;
        break;
    }

    case TATAMI_LIMIT_SNAKE: 
    {
    Sami->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
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
      int tickTurn = 0;
      display.clear();   
      display.drawString(19, 0, "Strategy Ronaldinho"); 
      display.drawString(0, 9, "---------------------"); 
      display.drawString(0,28, "Press Star()");    
      display.display();
      if (start->GetIsPress())
      {
        display.clear();   
        display.drawString(19, 0, "Strategy Ronaldinho"); 
        display.drawString(0, 9, "---------------------"); 
        display.drawString(0,28, "Ok Gaucho");    
        display.display();
        delay(5000);
        ronaldinho = GO_FORWARD_RONALDINHO;
      } 
      else Sami->Stop();
      break;
    }

    case GO_FORWARD_RONALDINHO:
    {
      Sami->Forward(AVERAGE_SPEED, AVERAGE_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) ronaldinho = OLEE_RONALDINHO;
      break;
    }

    case OLEE_RONALDINHO:
    {
      Sami->Backward(ATTACK_SPEED, ATTACK_SPEED);
      delay(1000);
      ronaldinho = STOP_RONALDINHO;
      break;
    }

    case STOP_RONALDINHO:
    {
      Sami->Stop();
      delay(500);
      ronaldinho = SWITCH_STRATEGY_RONALDINHO;
      break;
    }

    case SWITCH_STRATEGY_RONALDINHO:
    {
      Sami->Stop();
      break;
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
    display.clear();   
    display.drawString(19, 0, "Strategy Veni Veni"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,28, "Press Star()");    
    display.display();
    if (start->GetIsPress())
    {
      display.clear();   
      display.drawString(19, 0, "Strategy Veni Veni"); 
      display.drawString(0, 9, "---------------------"); 
      display.drawString(0,28, "Dale dale veni");    
      display.display();
      delay(5000);
      veniVeni = SEARCH_VENI_VENI;
    } 
    else Sami->Stop();
    break;
    }

    case SEARCH_VENI_VENI:
    {
      Sami->Right(ATTACK_SPEED, ATTACK_SPEED);
      delay(tickTurn);
      Sami->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
      break;    
    }

    case TURN_RIGHT_VENI_VENI:
    {
      Sami->Right(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
      if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
      break;
    }

    case TURN_LEFT_VENI_VENI:
    {
      Sami->Left(SEARCH_SPEED, SEARCH_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
      if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
      if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
      if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
      break;
    }

    case ATTACK_VENI_VENI:
    {
      if(ldr < MONTADO){
        Sami->Forward(ATTACK_SPEED, ATTACK_SPEED);
        if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
      }

      else 
      {
        Sami->Stop();
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
      }
      break;
    }

    case TATAMI_LIMIT_VENI_VENI: 
    {
    Sami->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
    delay(300);

    if(leftTatamiRead > 250 && righTatamiRead > 250) veniVeni = SEARCH_VENI_VENI;
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
      Sami->Stop();
      display.clear();   
      display.drawString(19, 0, "Strategy River"); 
      display.drawString(0, 9, "---------------------"); 
      display.drawString(0,28, "Press Star()");    
      display.display();
      if (start->GetIsPress())
      {
        display.clear();   
        display.drawString(19, 0, "Strategy River"); 
        display.drawString(0, 9, "---------------------"); 
        display.drawString(0,28, "El taco no, hace personal");
        display.display();
        delay(5000);
        Sami->Right(ATTACK_SPEED, ATTACK_SPEED);
        delay(tickTurn);
        river = SEARCH_VENI_VENI;
      }  
      break;
    }
    
    case SEARCH_RIVER:
    {
      Sami->Right(AVERAGE_SPEED, AVERAGE_SPEED);
      if(distSharpLeft < RIVAL || distSharpRigh < RIVAL) river = ATTACK_RIVER;
      break;
    }

    case ATTACK_RIVER:
    {
      Sami->Forward(ATTACK_SPEED, ATTACK_SPEED);
      if(leftTatamiRead < 250 || righTatamiRead < 250) river = TATAMI_LIMIT_RIVER;
      break;
    }

    case TATAMI_LIMIT_RIVER:
    {
      Sami->Backward(AVERAGE_SPEED, AVERAGE_SPEED);
      delay(300);
      river = SEARCH_RIVER;
      break;
    }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum sanLorenzo
{
  STANDBY_SAN_LORENZO
};
int sanLorenzo = STANDBY_SAN_LORENZO;
//Maquina de estados para la estrategia de San Lorenzo (literalmente no hace nada)
void SanLorenzo()
{
  switch (sanLorenzo)
  {
  case STANDBY_SAN_LORENZO:
  {
  display.clear();   
  display.drawString(19, 0, "TE EQUIVOCASTE..."); 
  display.drawString(0, 9, "PELOTUDO..."); 
  display.drawString(0,28, "Press Star()");    
  display.display();
  Sami->Stop();
  delay(5000);
  Sami->Forward(ATTACK_SPEED, ATTACK_SPEED);
  delay(5000);
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
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.display();
    if(selectStrategy->GetIsPress()) repositioningMenu = TURN_FRONT;
    if(start->GetIsPress()) 
    {
      tickTurn = 0;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case TURN_FRONT:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,37, "The rival is ahead"); 
    display.display();
    if(selectStrategy->GetIsPress()) repositioningMenu = TURN_SIDE;
    if(start->GetIsPress())
    {
      tickTurn = TICK_TURN_FRONT;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case TURN_SIDE:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,37, "Turn 90 degrees"); 
    display.display();
    if(selectStrategy->GetIsPress()) repositioningMenu = SHORT_BACK_TURN;
    if(start->GetIsPress())
    {
      tickTurn = TICK_TURN_SIDE;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case SHORT_BACK_TURN:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,37, "The rival is a little behind"); 
    display.display();
    if(selectStrategy->GetIsPress()) repositioningMenu = LONG_BACK_TURN;
    if(start->GetIsPress())
    {
      tickTurn = TICK_SHORT_BACK_TURN;
      strategy = STRATEGIES_MENU;
    }
    break;
  }
  case LONG_BACK_TURN:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,37, "the rival is far behind"); 
    display.display();
    if(selectStrategy->GetIsPress()) repositioningMenu = TURN_FRONT;
    if(start->GetIsPress())
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
  SNAKE_MENU,
  RONALDINHO_MENU,
  VENI_VENI_MENU,
  RIVER_MENU,
  SAN_LORENZO_MENU
};
int menu = MAIN_MENU;
//Maquina de estados para navegar dentro del menu y seleccionar la estrategia
void StrategiesMenu()
{
  switch (menu)
  {
  case MAIN_MENU:
  {
    display.clear();   
    display.drawString(19, 0, "Select strategy"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,19, "Snake");   
    display.drawString(0,28, "Ronaldinho");    
    display.drawString(0,37, "Veni veni");  
    display.drawString(0,46, "River");    
    display.drawString(0,55, "San Lorenzo");
    display.display();
    if(selectStrategy->GetIsPress()) menu = SNAKE_MENU;
    break;
  }
  
  case SNAKE_MENU:
  { 
    display.clear();
    display.drawString(19, 0, "Select strategy"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,19, "Snake");  
    display.display();
    if(selectStrategy->GetIsPress()) menu = RONALDINHO_MENU;
    if(start->GetIsPress()) strategy = SNAKE;
    break;
  }

  case RONALDINHO_MENU:
  {  
    display.clear();
    display.drawString(19, 0, "Select strategy"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,28, "Ronaldinho");  
    display.display();
    if(selectStrategy->GetIsPress()) menu = VENI_VENI_MENU;
    if(start->GetIsPress()) strategy = RONALDINHO;
    break;
  }

  case VENI_VENI_MENU:
  { 
    display.clear();
    display.drawString(19, 0, "Select strategy"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,37, "Veni veni"); 
    display.display();
    if(selectStrategy->GetIsPress()) menu = RIVER_MENU;
    if(start->GetIsPress()) strategy = VENI_VENI;
    break;
  }

  case RIVER_MENU:
  {
    display.clear();
    display.drawString(19, 0, "Select strategy"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,46, "River");     
    display.display();
    if(selectStrategy->GetIsPress()) menu = SAN_LORENZO_MENU;
    if(start->GetIsPress()) strategy = RIVER;
    break;
  }

  case SAN_LORENZO_MENU:
  { 
    display.clear();
    display.drawString(19, 0, "Select strategy"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,55, "San Lorenzo");  
    display.display();
    if(selectStrategy->GetIsPress()) menu = SNAKE_MENU;
    if(start->GetIsPress()) strategy = SAN_LORENZO;
    break;
  }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
void logicMovement(){
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
    case SNAKE:
    {
      Snake();
      break;
    }

    case RONALDINHO:
    {
      Ronaldinho();
      if(ronaldinho = SWITCH_STRATEGY_RONALDINHO) strategy = secondaryStrategy;
      break;
    }

    case VENI_VENI:
    {
      VeniVeni();
      break;
    }

    case RIVER:
    {
      River();
      break;
    }
    case SAN_LORENZO:
    {
      SanLorenzo();
      break;
    }    
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//Funcion para imprimir la estrategia y el caso en el puerto Bluetooth
void printStrategy() 
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
}
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
  if(DEBUG_STATE) printStrategy();
  if(DEBUG_LDR) printLdr();
}