#define TATAMI_IZQ A0
#define TATAMI_DER A7
#define BUZZER 5

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
  //int tatami_izquierdo = leer_tatami(TATAMI_IZQ);
 
  if (/*tatami_izquierdo < 250 ||*/ tatami_derecho < 250 ){
    BuzzerOn();    
    Serial.println("Detecta");
  }
  else{
    BuzzerOff();
    Serial.println("no Detecta ");

  }
  //Serial.print(tatami_izquierdo);
  //Serial.print("||");
  Serial.println(tatami_derecho);
  delay(500);
  }gitgit