#define DEBUG_TATAMI 1
#define DEBUG_MOVIMIENTOS 1
//Sensors fo tatami
#define PIN_SENSOR_TATAMI_IZQ A6
#define PIN_SENSOR_TATAMI_DER A1
//sensor de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO A0
#define PIN_SENSOR_DISTANCIA_IZQUIERDO A1
// motor
#define PIN_MOTOR_MR1 11 //DIR
#define PIN_MOTOR_MR2PWM 10 //PWM
#define PIN_MOTOR_ML1 9 //DIR
#define PIN_MOTOR_ML2PWM 6 //PWM
#define PIN_BUTTON_START 2
#define PIN_BUTTON_STRATEGY 3  //te ponen 
#define PIN_BUZZER 5
#define VELOCIDAD_DERECHA 100
#define VELOCIDAD_IZQUIERDA 100 
#define BORDE_TATAMI 250
int tatami_derecho;
int tatami_izquierdo;
bool sensorIzquierdo;
bool sensorDerecho;
#define TICK 1000;
unsigned long tiempo_actual = 0;
class Motor
{
    //atributos
private:
    int velocidad;
    int pin_direccion;
    int pin_pwm;

public:

    Motor(int dir, int pwm, int vel){
        pin_direccion = dir;
        pin_pwm = pwm;
        velocidad = vel;

        pinMode(pin_direccion, OUTPUT);
        pinMode(pin_pwm, OUTPUT);
    }
//metodos
    void setVelocidad(int v){
        velocidad=v;
    } 
    void forward (){
        digitalWrite(pin_direccion, HIGH);
        analogWrite(pin_pwm, velocidad);
    }
    void backward (){
        digitalWrite(pin_direccion, LOW);
        analogWrite(pin_pwm, velocidad);
    }
    void stop (){
        digitalWrite(pin_direccion, LOW );
        analogWrite(pin_pwm, 0);
    } 
          
};
class sensorTatami{
private:
    int pin;

public:
     sensorTatami(int p){
        pin = p;
        pinMode(pin, INPUT);
    }
    int leerSensorTatami(){
        return analogRead(pin); 
    }
};
class sensorDistancia{
private:
    int pin;

public:
     sensorDistancia(int p){
        pin = p;
        pinMode(pin, INPUT);
    }
    bool leerSensorDistancia(){
        return digitalRead(pin);
    }
};
class Buzzer {
  private:
    int pin;

  public:
    Buzzer(int p) {
      pin = p;
      pinMode(pin, OUTPUT);
    }
    void setPrenderBuzzer() {
      digitalWrite(pin, HIGH);
    }
    void setApagarBuzzer() {
      digitalWrite(pin, LOW);
    }

};
class Button {
  private:
    int pin = 9;
    bool state = HIGH;

    //metodo
  public:
    Button(int p) {
      pin = p;

      pinMode(pin, INPUT_PULLUP);
    }

    //metodos o acciones
    bool getIsPress() {
      bool estado = digitalRead(pin);
      return estado;
    }

};

//Instancio los motores
Motor m1 = Motor(PIN_MOTOR_MR1, PIN_MOTOR_MR2PWM, VELOCIDAD_DERECHA);
Motor m2 = Motor(PIN_MOTOR_ML1, PIN_MOTOR_ML2PWM, VELOCIDAD_IZQUIERDA);
//Instancio los sensors
sensorTatami tatami_der = sensorTatami(PIN_SENSOR_TATAMI_DER);
sensorTatami tatami_izq = sensorTatami(PIN_SENSOR_TATAMI_IZQ);

sensorDistancia distanciaDerecha = sensorDistancia(PIN_SENSOR_DISTANCIA_DERECHO);
sensorDistancia distanciaIzquierda = sensorDistancia(PIN_SENSOR_DISTANCIA_IZQUIERDO);

//Instancio los buttons *punteros
Button *strategy = new  Button(PIN_BUTTON_STRATEGY);
Button *start = new  Button(PIN_BUTTON_START);
//Instancio los buzzers
Buzzer *b1 = new Buzzer(PIN_BUZZER);   
//motores
void forward() //voy hacia adelante
{
  m1.setVelocidad(230);
  m2.setVelocidad(230);
  m1.forward();
  m2.forward();
}

void backward() //voy hacia atras
{
  m1.setVelocidad(230);
  m2.setVelocidad(230);
  m1.backward();
  m2.backward();
}
 
void left() //giro a la izquierda
{
  m1.setVelocidad(230);
  m2.setVelocidad(230);
  m1.forward();
  m2.backward();
}


void right() //giro a la derecha
{
  m1.setVelocidad(230);
  m2.setVelocidad(230);
  m1.backward();
  m2.forward();
}

void stopMotor() //freno
{
  m1.setVelocidad(0);
  m2.setVelocidad(0);
  m1.stop();
  m2.stop();
}
//buzzers
void BuzzerOn(){
    b1->setPrenderBuzzer();
}
void BuzzerOff(){
    b1->setApagarBuzzer();
}
//botones

enum{
  STANDBY,
  BUSQUEDA,
  HAY_RIVAL,
  HAY_BORDE,
};
int movimientos = STANDBY;
void switchCase(){
switch (movimientos)
{
case STANDBY:
{
  bool boton_start = start->getIsPress();
  if (!boton_start) {
    delay(5000);
    movimientos = BUSQUEDA; 
  }
  else{
      stopMotor();
  }
  break;
}  
  

case BUSQUEDA:{
    bool boton_strategy = strategy->getIsPress();
    left();
    if (!boton_strategy ) movimientos = HAY_BORDE;
    if (!sensorIzquierdo == true || !sensorDerecho == true) movimientos = HAY_RIVAL;
    
    break;
}

case HAY_RIVAL: {
  bool boton_strategy = strategy->getIsPress();
  forward();
  if (!boton_strategy) movimientos =HAY_BORDE;
  
  if (!sensorIzquierdo != true || !sensorDerecho != true) movimientos = BUSQUEDA;

  break;
}

case HAY_BORDE: {
    backward();
    delay(1000);
    movimientos = BUSQUEDA; 
     break;
}
}
}

//-------------------------------------------------------------
void ImprimirEstadoRobot(int movement) 
{
  String estado_robot = "";
  if (movement == STANDBY)
    {estado_robot = "Stand By";}
   else if (movement == BUSQUEDA)
   {estado_robot = "Buscando";} 
  else if (movement == HAY_RIVAL)
    {estado_robot = "HAY RIVAL";}
  else if (movement == HAY_BORDE)
    {estado_robot = "HAY BORDE";}


  Serial.print("State: ");
  Serial.print(estado_robot);
  Serial.print(" || ");
  Serial.print(sensorDerecho);
  Serial.print(" || ");
  Serial.print(sensorIzquierdo);
  Serial.println(" || ");
}  
void setup() {
    Serial.begin(9600);
    
}

void loop(){
    sensorIzquierdo = distanciaIzquierda.leerSensorDistancia();
    sensorDerecho = distanciaDerecha.leerSensorDistancia();
    tatami_derecho = tatami_der.leerSensorTatami();
    tatami_izquierdo = tatami_izq .leerSensorTatami();

     switchCase();

  /*if (millis() > tiempo_actual + TICK)
  {
    tiempo_actual = millis();*/
    if(DEBUG_MOVIMIENTOS){
      ImprimirEstadoRobot(movimientos);
    }
    /*
    if(DEBUG_TATAMI){
        Serial.print(tatami_izquierdo);
        Serial.print("||");
        Serial.println(tatami_derecho);
    }*/
  }