#include <Arduino.h>
#include "IncRotaryEncoder.h"

const uint8_t CLK_PIN = 2;
const uint8_t DT_PIN  = 3;

IncRotaryEncoder incRotEn(CLK_PIN, DT_PIN);

// value could be for example a set temperature of an oven
int value;
// increment per step
const int stepSize = 1;
// sensitivity of the encoder
const float sensitivity = 0.05f;

// step callback function
void onStepFunc(int direction, int stepsPerSecond)
{
    // update value logarithmically for smoother control
    // this way turning the encoder faster results in a faster change of the value
    // useful if the range of value is big
    // in this example a base of 2 is used but could be any base really
    float base;
    base = 2.0;
    
    // check direction
    if (direction == ENCODER_CW) {
        value += stepSize * powf(base, stepsPerSecond * sensitivity);
    } else {
        value -= stepSize * powf(base, stepsPerSecond * sensitivity);
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

