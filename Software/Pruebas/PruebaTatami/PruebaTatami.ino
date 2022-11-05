#include <Tatami.h>

//Sensors de tatami
#define PIN_SENSOR_TATAMI_IZQ 32
#define PIN_SENSOR_TATAMI_DER 33
int righTatamiRead;
int leftTatamiRead;
#define BORDE_TATAMI 300

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
    righTatamiRead = rightTatami->TatamiRead();
    leftTatamiRead = LeftTatami->TatamiRead();
    if(DEBUG)
    {
        if (millis() > tiempo_actual + TICK_DEBUG)
        {
            Serial.print("Right tatami: ");
            Serial.println(righTatamiRead);
            Serial.print("Left tatami: ");
            Serial.println(leftTatamiRead);
        }

    }
}
