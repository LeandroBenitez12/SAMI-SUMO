#include <Sharp.h>

//sensor de distancia
#define PIN_SENSOR_DISTANCIA_DERECHO 32
#define PIN_SENSOR_DISTANCIA_IZQUIERDO 33
#define RIVAL 30
int distSharpRigh;
int distSharpLeft;

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
    distSharpRigh = sharpRight->SharpDist();
    distSharpLeft = sharpLeft->SharpDist();
    
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
