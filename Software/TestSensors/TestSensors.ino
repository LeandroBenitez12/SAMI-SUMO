#define TATAMI_IZQ A7
#define TATAMI_DER A6
#define BUZZER 5
#define MR1 11 //DIR
#define MR2PWM 10 //PWM
#define ML1 9 //DIR
#define ML2PWM 6 //PWM

int valor_pwm = 100;

void AsignacionpinesMOTORES()
{
  pinMode(MR1, OUTPUT);
  pinMode(MR2PWM, OUTPUT);
  pinMode(ML1, OUTPUT);
  pinMode(ML2PWM, OUTPUT);
}

void Derecha() {
  digitalWrite(MR1, HIGH);
  analogWrite(MR2PWM, valor_pwm);
  digitalWrite(ML1, HIGH);
  analogWrite(ML2PWM, valor_pwm);
  
  }

void Izquierda() {
  digitalWrite(MR1, LOW);
  analogWrite(MR2PWM, valor_pwm);
  digitalWrite(ML1, LOW);
  analogWrite(ML2PWM, valor_pwm);
  
  }
  void Atras() {
  digitalWrite(MR1, HIGH);
  analogWrite(MR2PWM, valor_pwm);
  digitalWrite(ML1, LOW);
  analogWrite(ML2PWM, valor_pwm);
  }
  void Adelante() {
  digitalWrite(MR1, LOW);
  analogWrite(MR2PWM, valor_pwm);
  digitalWrite(ML1, HIGH);
  analogWrite(ML2PWM, valor_pwm);
  }
  void Stop() {
  digitalWrite(MR1, LOW);
  analogWrite(MR2PWM, 0);
  digitalWrite(ML1, LOW);
  analogWrite(ML2PWM, 0);
  }
void BuzzerOn()
{
  digitalWrite(BUZZER, HIGH);
}
void BuzzerOff()
{
  digitalWrite(BUZZER, LOW);
}

int leer_tatami(int pin_tatami){
    int tatami = analogRead(pin_tatami);
    return tatami;
} 

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  int tatami_derecho = leer_tatami(TATAMI_DER);
  int tatami_izquierdo = leer_tatami(TATAMI_IZQ);
 
  if (tatami_izquierdo < 250 && tatami_derecho < 250 ){
    BuzzerOn();    
    Atras();
    delay(1000);
    Derecha();
    delay(500);
  }
  else{
    BuzzerOff();
    Adelante();
  }
  Serial.print(tatami_izquierdo);
  Serial.print("||");
  Serial.println(tatami_derecho);
  }
