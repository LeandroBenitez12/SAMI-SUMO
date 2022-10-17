#include <Sharp.h>

//sensor de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO A3
#define PIN_SENSOR_DISTANCIA_IZQUIERDO A4
#define RIVAL 30
int distSharpRigh;
int distSharpLeft;
int n = 3;

unsigned long tiempo_actual = 0;
#define TICK_DEBUG 500
#define DEBUG 1

Sharp *sharpRight = new Sharp(PIN_SENSOR_DISTANCIA_DERECHO);
Sharp *sharpLeft = new Sharp(PIN_SENSOR_DISTANCIA_IZQUIERDO);

void setup()
{
    Serial.begin(9600);
}

void loop() 
{
    distSharpRigh = sharpRight->SharpDist(n);
    distSharpLeft = sharpLeft->SharpDist(n);
    
    if(DEBUG)
    {
        if (millis() > tiempo_actual + TICK_DEBUG)
        {
            Serial.print("Right dist: ");
            Serial.println(distSharpRigh);
            Serial.print("Left dist: ");
            Serial.println(distSharpLeft);
        }

    }
  
}
