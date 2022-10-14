#define PIN_BUTTON_START 34
bool boton_start;
/*#include <Button.h>


Button *start = new  Button(PIN_BUTTON_START);
*/

class Button
{
private:
  int pin = 9;
  bool state = HIGH;

  // metodo
public:
  Button(int p)
  {
    pin = p;

    pinMode(pin, INPUT);
  }

  // metodos o acciones
  bool getIsPress()
  {
    bool estado = digitalRead(pin);
    return estado;
  }
};

Button *start = new Button(PIN_BUTTON_START);

void setup()
{
    Serial.begin(9600);
}
void loop()
{
    boton_start = start->getIsPress();
    if (!boton_start)
    {
      Serial.println("SI");
    }
    else
    {
      Serial.println("NO");
    }
}
