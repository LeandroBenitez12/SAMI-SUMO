#include "Tatami.h"


Tatami::Tatami(int p) 
{
    pin = p;
    pinMode(pin, INPUT);
}

<<<<<<< HEAD
float Tatami::TatamiRead(int n) 
=======
float Tatami::TatamiRead() 
>>>>>>> 84d439c4c9d53cc8b836e205e0ca530ee9426de7
{
    long suma = 0;
    //if (pin == PIN_TATAMI_D) suma = -500;
    float num, den;
    for (int i = 0; i < n; i++)  // Realizo un promedio de "n" valores
    {
      suma = suma + analogRead(pin);
    }
    float lectura = suma / n;
    lectura = lectura * 1000 / 1024.0;                   //convierte el adc en valores de 0 a 1000
    if (lectura < minimoTatami) minimoTatami = lectura;  // Actualizo minimo
    if (lectura > maximoTatami) maximoTatami = lectura;  // Actualizo maximo
    //  num = lectura - minimoTatami;
    //  num *= 1000.0;
    //  den = maximoTatami - minimoTatami;
    //  lectura = (float) num / den;
    return (lectura);
}
