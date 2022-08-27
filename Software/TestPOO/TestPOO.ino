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
    int pin_senal;

public:
     Sensor(int p){
        pin_senal = p;
        pinMode(pin_senal, INPUT);
    }
    int leerSensor(int pin_tatami){
        pin_senal = pin_tatami;
        int tatami = analogRead(pin_senal);
        return tatami;
    } 
};
// motor
#define TATAMI_IZQ A7
#define TATAMI_DER A6
#define BUZZER 5
#define MR1 11 //DIR
#define MR2PWM 10 //PWM
#define ML1 9 //DIR
#define ML2PWM 6 //PWM
int velocidad_derecha = 100;
int velocidad_izquierda = 100;

//Instancio los motores
Motor m1 = Motor(MR1, MR2PWM, velocidad_derecha);
Motor m2 = Motor(ML1, ML2PWM, velocidad_izquierda);

Sensor tatami_dere = Sensor(TATAMI_DER);
Sensor tatami_izqu = Sensor(TATAMI_IZQ);

/*
int Sensorderecho(){
    int state = tatami_derecho.leerSensor();
    return state;
}
int Sensorizquierdo(){
    int state = tatami_izquierdo.leerSensor();
    return state;
}*/

void forward() //voy hacia adelante
{
    m1.forward();
    m2.forward();
}

void backward() //voy hacia atras
{
    m1.backward();
    m2.backward();
}
 
void left() //giro a la izquierda
{
    m1.forward();
    m2.backward();
}


void right() //giro a la derecha
{
    m1.backward();
    m2.forward();
}

void stopMotor() //freno
{
  m1.stop();
  m2.stop();
}
void setup() {

}
void loop(){
  int tatami_derecho = tatami_dere.leerSensor(TATAMI_DER);
  int tatami_izquierdo = tatami_izqu .leerSensor(TATAMI_IZQ);
 
  if (tatami_izquierdo < 250 && tatami_derecho < 250 ){
    //BuzzerOn();    
    backward();
    delay(1000);
    right();
    delay(500);
  }
  else{
    //BuzzerOff();
    forward();
  }
  Serial.print(tatami_izquierdo);
  Serial.print("||");
  Serial.println(tatami_derecho);
}
