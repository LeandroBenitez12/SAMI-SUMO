#include "Sharp.h"


Sharp::Sharp(int p)
{
    pin = p;
    pinMode(pin, INPUT);
}

double Sharp::SharpDist()
{
    long suma = 0;
    for (int i = 0; i < n; i++) // Realizo un promedio de "n" valores
    {
        suma = suma + analogRead(pin);
    }
    float adc = suma / n;
    // float distancia_cm = 17569.7 * pow(adc, -1.2062);
    if (adc < 400)
        adc = 400;
    // float distancia_cm = 2076.0 / (adc - 11.0);
    // Formula para el sensor GP2Y0A60SZLF
    // https://www.instructables.com/How-to-setup-a-Pololu-Carrier-with-Sharp-GP2Y0A60S/
    double distancia_cm = 187754 * pow(adc, -1.183);//REGULAR LA POTENCIA PARA OBETENER BUENA PRESICION
    return (distancia_cm);
    delay(100);
}
