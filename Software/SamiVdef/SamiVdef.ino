//librerias
#include <Button.h>
#include <MotorSumo.h>
#include <Tatami.h>
#include <Sharp.h>
#include "BluetoothSerial.h"
#include <SSD1306.h>

//debug
#define DEBUG_SHARP 0
#define DEBUG_TATAMI 0
#define DEBUG_STATE 0
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
#define PIN_SENSOR_TATAMI_IZQ 26
#define PIN_SENSOR_TATAMI_DER 27
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 300

//Variables y constantes para los sensores de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO 25
#define PIN_SENSOR_DISTANCIA_IZQUIERDO 33
#define RIVAL 30
int distSharpRigh;
int distSharpLeft;

//Variables y constantes LDR
#define PIN_SENSOR_LDR 32
#define MONTADO 100
int ldr;

// Variables y constantes para los motores
#define PIN_MOTOR_MR1 22 //DIR
#define PIN_MOTOR_MR2PWM 21 //PWM
#define PIN_MOTOR_ML1 19 //DIR
#define PIN_MOTOR_ML2PWM 18 //PWM
#define SEARCH_SPEED 120
#define ATTACK_SPEED 250
#define ATTACK_SPEED_SNAKE 255
#define AVERAGE_SPEED 255
int righSpeed = 255;
int leftSpeed = 255;

//Giros de reposicion
int tickTurn = 0;
#define TICK_TURN_45 70
#define TICK_TURN_90 140
#define TICK_TURN_135 210


//Pines para los botones
#define PIN_BUTTON_START 34
bool boton_start;
#define PIN_BUTTON_STRATEGY 35

// variables y constantes para la pantalla oled
#define PIN_SDA 16
#define PIN_SCL 17
//<------------------------------------------------------------------------------------------------------------->//
//Instanciamos todos los objetos del robot
SSD1306 display (0x3C,PIN_SDA, PIN_SCL); // inicializa pantalla con direccion 0x3C

Motor *mDer = new Motor(PIN_MOTOR_MR1, PIN_MOTOR_MR2PWM, righSpeed);
Motor *mIzq = new Motor(PIN_MOTOR_ML1, PIN_MOTOR_ML2PWM, leftSpeed);

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
void printLDR()
{
   SerialBT.print("LDR: ");
   SerialBT.println(ldr);  
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
  MENU,
  SNAKE,
  RONALDINHO,
  VENI_VENI,
  RIVER,
  SAN_LORENZO
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
        boton_start = start->GetIsPress();
        display.clear();   
        display.drawString(19, 0, "Strategy Snake"); 
        display.drawString(0, 9, "---------------------"); 
        display.drawString(0,28, "Press Star()");    
        display.display();
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
        /*righSpeed = ATTACK_SPEED;
        leftSpeed = ATTACK_SPEED;
        right();
        delay(tickTurn);*/
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        right();
        if(leftTatamiRead < 250 || righTatamiRead < 250) snake = TATAMI_LIMIT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) snake = TURN_RIGHT_SNAKE;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) snake = TURN_LEFT_SNAKE;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) snake = ATTACK_SNAKE;
        break;
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
        break;
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
        break;
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
        break;
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
      boton_start = start->GetIsPress();
      display.clear();   
      display.drawString(19, 0, "Strategy Ronaldinho"); 
      display.drawString(0, 9, "---------------------"); 
      display.drawString(0,28, "Press Star()");    
      display.display();
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
      break;
    }

    case STOP_RONALDINHO:
    {
      stop();
      delay(2000);
      ronaldinho = SWITCH_STRATEGY_RONALDINHO;
      break;
    }

    case SWITCH_STRATEGY_RONALDINHO:
    {
      stop();
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
        boton_start = start->GetIsPress();
        display.clear();   
        display.drawString(19, 0, "Strategy Veni Veni"); 
        display.drawString(0, 9, "---------------------"); 
        display.drawString(0,28, "Press Star()");    
        display.display();
        if (!boton_start) 
        {
          display.clear();   
        display.drawString(19, 0, "Strategy Veni Veni"); 
        display.drawString(0, 9, "---------------------"); 
        display.drawString(0,28, "arranca en 5");    
        display.display();
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
        /*righSpeed = ATTACK_SPEED;
        leftSpeed = ATTACK_SPEED;
        right();
        delay(tickTurn);*/
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        left();
        if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
        break;    
    }

    case TURN_RIGHT_VENI_VENI:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        stop();
        if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
        break;
    }

    case TURN_LEFT_VENI_VENI:
    {
        righSpeed = SEARCH_SPEED;
        leftSpeed = SEARCH_SPEED;
        stop();
        if(leftTatamiRead < 250 || righTatamiRead < 250) veniVeni = TATAMI_LIMIT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft <= RIVAL) veniVeni = ATTACK_VENI_VENI;
        break;
    }

    case ATTACK_VENI_VENI:
    {
      righSpeed = ATTACK_SPEED;
      leftSpeed = ATTACK_SPEED;
      if(ldr < MONTADO/*distSharpRigh <= 15 && distSharpLeft <= 15*/){
        forward();
        } 
      else 
      {
        stop();
        if(distSharpRigh > RIVAL && distSharpLeft > RIVAL) veniVeni = SEARCH_VENI_VENI;
        if(distSharpRigh <= RIVAL && distSharpLeft > RIVAL) veniVeni = TURN_RIGHT_VENI_VENI;
        if(distSharpRigh > RIVAL && distSharpLeft <= RIVAL) veniVeni = TURN_LEFT_VENI_VENI;
      }
      break;
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
      boton_start = start->GetIsPress();
      display.clear();   
      display.drawString(19, 0, "Strategy RIver"); 
      display.drawString(0, 9, "---------------------"); 
      display.drawString(0,28, "Press Star()");    
      display.display();
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
      /*righSpeed = ATTACK_SPEED;
        leftSpeed = ATTACK_SPEED;
        right();
        delay(tickTurn);*/
      righSpeed = AVERAGE_SPEED;
      leftSpeed = AVERAGE_SPEED;
      right();
      if(distSharpLeft > RIVAL) river = ATTACK_RIVER;
      break;
    }

    case ATTACK_RIVER:
    {
      righSpeed = ATTACK_SPEED;
      leftSpeed = ATTACK_SPEED;
      forward();
      if(leftTatamiRead < 250 || righTatamiRead < 250) river = TATAMI_LIMIT_RIVER;
      break;
    }

    case TATAMI_LIMIT_RIVER:
    {
      righSpeed = AVERAGE_SPEED;
      leftSpeed = AVERAGE_SPEED;
      backward;
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
  stop();
  delay(5000);
  forward();
  delay(1000);
  break;
  }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
enum menu
{
  /*SELECT_TURN_MENU,
  TURN_45,
  TURN_90,
  TURN_135,*/
  MAIN_MENU,
  SNAKE_MENU,
  RONALDINHO_MENU,
  VENI_VENI_MENU,
  RIVER_MENU,
  SAN_LORENZO_MENU
};
int menu = MAIN_MENU;
//Maquina de estados para navegar dentro del menu y seleccionar la estrategia
int strategySelect;
void Menu()
{
  switch (menu)
  {

  /*case SELECT_TURN_MENU:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,19, "Turn 45g");   
    display.drawString(0,28, "Turn 90g");    
    display.drawString(0,37, "Turn 135g");
    display.display();
    if(selectStrategy->GetIsPress()) menu = TURN_45;
    if(start->GetIsPress())
    {
      strategy = strategySelect;
    }
    break;
  }

  case TURN_45:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,19, "Turn 45g");   
    display.display();
    if(selectStrategy->GetIsPress()) menu = TURN_90;
    if(start->GetIsPress())
    {
      tickTurn = TICK_TURN_45;
      strategy = strategySelect;
    }
    break;
  }

  case TURN_90:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); display.drawString(0,37, "Turn 135g");
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,28, "Turn 90g");  
    display.display();
    if(selectStrategy->GetIsPress()) menu = TURN_135;
    if(start->GetIsPress())
    {
      righSpeed = ATTACK_SPEED;
      leftSpeed = ATTACK_SPEED;
      right();
      tickTurn = TICK_TURN_90;
      strategy = strategySelect;
    }
    break;
  }

  case TURN_135:
  {
    display.clear();   
    display.drawString(19, 0, "Select Turn"); 
    display.drawString(0, 9, "---------------------"); 
    display.drawString(0,37, "Turn 135g");  
    display.display();
    if(selectStrategy->GetIsPress()) menu = TURN_45;
    if(start->GetIsPress())
    {
      tickTurn = TICK_TURN_135;
      strategy = strategySelect;
    }
    break;
  }*/
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
    if(start->GetIsPress()){
      display.clear();
      display.drawString(0,28, "OK, Pecador"); 
      display.display();
      strategy = SNAKE;
    }
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
    if(start->GetIsPress()){ 
      display.clear();
      display.drawString(0,28, "OK, GAUCHO"); 
      display.display();
      strategy = RONALDINHO;
    }
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
    if(start->GetIsPress()){
      
      display.clear();
      display.drawString(0,28, "OK, VENI veni"); 
      display.display();
      strategy = VENI_VENI;
    }
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
    if(start->GetIsPress())
      {
      display.clear();
      display.drawString(0,28, "El taco no, hace la personal"); 
      display.display();
      strategy = RIVER;
    }
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
    if(start->GetIsPress())
      {
      display.clear();
      display.drawString(0,28, "san lorenzo"); 
      display.drawString(0,36, "vos sos ortiva"); 
      display.display();
      strategy = SAN_LORENZO;
    }
    break;
  }
  }
}
//<------------------------------------------------------------------------------------------------------------->//
//<------------------------------------------------------------------------------------------------------------->//
//Maquina de estados para el menu de la pantalla oled
void logicMovement(){
  switch (strategy)
  {
    case MENU:
    {
      Menu();
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
  if(DEBUG_LDR) printLDR();
}
