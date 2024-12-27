#include <Arduino.h>
#include "IncRotaryEncoder.h"

const uint8_t CLK_PIN = 2;
const uint8_t DT_PIN  = 3;

IncRotaryEncoder incRotEn(CLK_PIN, DT_PIN);

// value could be for example a set temperature of an oven
int value;

// step callback function
void onStepFunc(int direction, int stepsPerSecond)
{
    // update value based on direction
    if (direction == ENCODER_CW) {
        value++;
    } else {
        value--;
    }

    Serial.print("Value: ");
    Serial.println(value);
}

void setup()
{
    Serial.begin(9600);

    // initialize the encoder module
    incRotEn.begin();
    // Set callback for on step
    incRotEn.setOnStepFunc(onStepFunc);

    value = 0;
}

void loop()
{
    // update the encoder
    incRotEn.update();
}

