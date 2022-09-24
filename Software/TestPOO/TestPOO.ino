#define DEBUG_TATAMI 0
#define DEBUG_movimientos 1
//Sensors fo tatami
#define PIN_SENSOR_TATAMI_IZQ A7
#define PIN_SENSOR_TATAMI_DER A6
// motor
#define PIN_MOTOR_MR1 11 //DIR
#define PIN_MOTOR_MR2PWM 10 //PWM
#define PIN_MOTOR_ML1 9 //DIR
#define PIN_MOTOR_ML2PWM 6 //PWM
#define PIN_BUTTON_START 2
<<<<<<< HEAD
#define PIN_BUTTON_STRATEGY 3  //te ponen 
=======
#define PIN_BUTTON_STRATEGY 3  //te ponen
>>>>>>> 8abab8ef5d2f6484b147afd6fb50e3164afb53dc
#define PIN_BUZZER 5
#define VELOCIDAD_DERECHA 100
#define VELOCIDAD_IZQUIERDA 100 
#define BORDE_TATAMI 250
int tatami_derecho;
int tatami_izquierdo;
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
class Sensor{
private:
    int pin;

public:
     Sensor(int p){
        pin = p;
        pinMode(pin, INPUT);
    }
    int leerSensor(){
        return analogRead(pin); 
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
Sensor tatami_dere = Sensor(PIN_SENSOR_TATAMI_DER);
Sensor tatami_izqu = Sensor(PIN_SENSOR_TATAMI_IZQ);
//Instancio los buttons
Button *strategy = new  Button(PIN_BUTTON_STRATEGY);
Button *start = new  Button(PIN_BUTTON_START);
//Instancio los buzzers
Buzzer *b1 = new Buzzer(PIN_BUZZER);   
//motores
void forward() //voy hacia adelante
{
    m1.setVelocidad(150); 
    m2.setVelocidad(150);
    m1.forward();
    m2.forward();
}

void backward() //voy hacia atras
{   m1.setVelocidad(200); 
    m2.setVelocidad(200);
    m1.backward();
    m2.backward();

}
 
void left() //giro a la izquierda
{
    m1.setVelocidad(100); 
    m2.setVelocidad(170);
    m1.forward();
    m2.backward();
}


void right() //giro a la derecha
{
    m1.setVelocidad(170); 
    m2.setVelocidad(100);
    m1.backward();
    m2.forward();
}

void stopMotor() //freno
{
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
    movimientos = BUSQUEDA; 
  }
  else{
      stopMotor();
<<<<<<< HEAD
  }
  break;
}  
  

case BUSQUEDA:{
  bool boton_strategy = strategy->getIsPress();
  right();
  if (!boton_strategy) {
    movimientos = HAY_RIVAL;
  }
  if (tatami_izquierdo < BORDE_TATAMI || tatami_derecho < BORDE_TATAMI ){
    movimientos =HAY_BORDE;
  }
  break;
}
case HAY_RIVAL: {
  forward();
  if (tatami_izquierdo < BORDE_TATAMI || tatami_derecho < BORDE_TATAMI ){
    movimientos =HAY_BORDE;
  }
  break;
}
case HAY_BORDE: {
  left();
  if (tatami_izquierdo > BORDE_TATAMI || tatami_derecho > BORDE_TATAMI ){
    movimientos =HAY_RIVAL;
  }
  break;
}
}
}
//-------------------------------------------------------------
void ImprimirEstadoRobot(int movement, int button_start) 
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
  Serial.print(button_start);
  Serial.println(" || ");
}
void setup() {
    Serial.begin(9600);
}

void loop(){
  tatami_derecho = tatami_dere.leerSensor();
  tatami_izquierdo = tatami_izqu .leerSensor();
 
  switchCase();
  
  if(DEBUG_movimientos){
    ImprimirEstadoRobot(movimientos,start->getIsPress());
  }
  if(DEBUG_TATAMI){
    Serial.print(tatami_izquierdo);
    Serial.print("||");
    Serial.println(tatami_derecho);
  }
=======
  }
  break;
}  
  

case BUSQUEDA:{
  bool boton_strategy = strategy->getIsPress();
  left();
  if (!boton_strategy) {
    movimientos = HAY_RIVAL;
  }
  if (tatami_izquierdo < BORDE_TATAMI && tatami_derecho < BORDE_TATAMI ){
    movimientos =HAY_BORDE;
  }
  break;
>>>>>>> 8abab8ef5d2f6484b147afd6fb50e3164afb53dc
}
case HAY_RIVAL: {
  forward();
  if (tatami_izquierdo < BORDE_TATAMI && tatami_derecho < BORDE_TATAMI ){
    movimientos =HAY_BORDE;
  }
  break;
}
case HAY_BORDE: {
  left();
  if (tatami_izquierdo > BORDE_TATAMI && tatami_derecho > BORDE_TATAMI ){
    movimientos =HAY_RIVAL;
  }
  break;
}
}
}
//-------------------------------------------------------------
void ImprimirEstadoRobot(int movement, int button_start) 
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
  Serial.print(button_start);
  Serial.println(" || ");
}
void setup() {
    Serial.begin(9600);
}

void loop(){
  tatami_derecho = tatami_dere.leerSensor();
  tatami_izquierdo = tatami_izqu .leerSensor();
 
  switchCase();
  
  if(DEBUG_movimientos){
    ImprimirEstadoRobot(movimientos,start->getIsPress());
  }
  if(DEBUG_TATAMI){
    Serial.print(tatami_izquierdo);
    Serial.print("||");
    Serial.println(tatami_derecho);
  }
}