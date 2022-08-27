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
class Sensores{
    private:
    int pin_señal
};
// motores
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
    forward();
    delay(1000);
    backward();
    delay(1000);
    left();
    delay(1000);
    right();
    delay(1000);
    stopMotor();
    delay(1000);
}
void loop(){
    
}
