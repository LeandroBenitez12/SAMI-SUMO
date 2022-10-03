#include <Tatami.h>

//Sensors de tatami
#define PIN_SENSOR_TATAMI_IZQ A0
#define PIN_SENSOR_TATAMI_DER A1
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 300
int n = 3;

unsigned long tiempo_actual = 0;
#define TICK_DEBUG 500
#define DEBUG 1

Tatami *rightTatami = new Tatami(PIN_SENSOR_TATAMI_DER);
Tatami *LeftTatami = new Tatami(PIN_SENSOR_TATAMI_IZQ);

void setup()
{
    Serial.begin(9600);
}

void loop() 
{
    righTatamiRead = rightTatami->TatamiRead(n);
    leftTatamiRead = LeftTatami->TatamiRead(n);
    if(DEBUG)
    {
        if (millis() > tiempo_actual + TICK_DEBUG)
        {
            Serial.print("Right tatami: ")
            Serial.println(righTatamiRead)
            Serial.print("Left tatami: ")
            Serial.println(leftTatamiRead)
        }

    }
}