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

void setup() {
  AsignacionpinesMOTORES();
  Adelante();
  delay(5000);
  Atras();
  delay(5000);
  Derecha();
  delay(5000);
  Izquierda();
  delay(5000);
  Stop();
  delay(5000);
}

void loop() {
  
}