#include <ButtonPULLUP.h>
#define PIN_BUTTON 18
bool lec;
bool flank;
unsigned long currentTimeButton = 0;
#define TICK_START 1000
Button *start = new Button(PIN_BUTTON);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  flank = start->GetIsPress();
  if(flank)
  {
    currentTimeButton = millis();
    lec = digitalRead(PIN_BUTTON);
    while(!lec)
      {
        lec = digitalRead(PIN_BUTTON);
        if(millis() > currentTimeButton + TICK_START)
        {
          Serial.println("joya");
          Serial.print("lectura: ");
          Serial.println(lec);
          Serial.print("flanco: ");
          Serial.println(flank);
        }
      }
      Serial.println("sali del while");
  }
  else Serial.println(" Boton no apretado");
  
}
