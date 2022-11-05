#define PIN_BUTTON_START 34
#define PIN_BUTTON_STRATEGY 35
bool boton_start;
bool boton_strategy;
/*#include <Button.h>


Button *start = new  Button(PIN_BUTTON_START);
*/

class Button
{
private:
  int pin;
  bool state = HIGH;

  // metodo
public:
  Button(int p)
  {
    pin = p;

    pinMode(pin, INPUT_PULL_DOWN);
  }

  // metodos o acciones
  bool getIsPress()
  {
    bool estado = digitalRead(pin);
    return estado;
  }
};

Button *start = new Button(PIN_BUTTON_START);
Button *strategy = new Button(PIN_BUTTON_STRATEGY);

void setup()
{
    Serial.begin(9600);
}
void loop()
{
    boton_start = start->getIsPress();
    boton_strategy = strategy->getIsPress();

    Serial.print(boton_start);
    Serial.print("  ||  ");
    Serial.println(boton_strategy);
}
