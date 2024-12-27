#include "IncRotaryEncoder.h"

// this look up table stores all valid encoder moves, the first two bits of the index are the previous CLK+DT pin outputs
// and the last two bits are the current CLK+DT pin outputs
// for more information
// https://www.best-microcontroller-projects.com/rotary-encoder.html#Taming_Noisy_Rotary_Encoders
const uint8_t ROTATION_LOOK_UP[] = {
    0x0, 0x1, 0x1, 0x0, 0x1, 0x0, 0x0, 0x1, 0x1, 0x0, 0x0, 0x1, 0x0, 0x1, 0x1, 0x0,
};

void defaultOnStepFunc(int direction, int stepsPerSecond) { (void)direction; (void)stepsPerSecond; }

/// @brief Initialize a Incremental Rotary Encoder
/// @param clkPin CLK pin on the module
/// @param dtPin DT pin on the module
/// @param dtIsHighOnClockwise wheter the DT pin is Low when a clockwise rotation is detected
IncRotaryEncoder::IncRotaryEncoder(uint8_t clkPin, uint8_t dtPin, bool dtIsHighOnClockwise) 
    : clkPin(clkPin),
      dtPin(dtPin),
      prevCurOutput(0),
      rotationCode(0),
      onStepFunc(defaultOnStepFunc),
      stepCount(0),
      speedPeriod(100),
      lastSpeedMeasurement(0),
      stepsPerSecond(0)
{
    rotationCodeCW  = dtIsHighOnClockwise ? 0x2b : 0x17;
    rotationCodeCCW = dtIsHighOnClockwise ? 0x17 : 0x2b;
}

/// @brief Set the function that is called when a step is detected
/// @param onStepFunc Function that is called
void IncRotaryEncoder::setOnStepFunc(OnStepFunc onStepFunc)
{
    this->onStepFunc = onStepFunc;
}

/// @brief Set the interval for speed measurement, every interval a speed approximation is made
/// @param interval Interval in milliseconds
void IncRotaryEncoder::setSpeedMeasurementInterval(uint16_t interval)
{
    this->speedPeriod = interval;
}

/// @brief Setup the pins of the module
/// @param useInternalPullup whether to use the internal pullup resistor for the pins
void IncRotaryEncoder::begin(bool useInternalPullup)
{
    // allow use of internal pullup resistor
    if (useInternalPullup) {
        pinMode(clkPin, INPUT_PULLUP);
        pinMode(dtPin, INPUT_PULLUP);
    } else {
        pinMode(clkPin, INPUT);
        pinMode(dtPin, INPUT);
    }
}

/// @brief Update the Rotary Encoder, check if a rotation occured and update the value accordingly
void IncRotaryEncoder::update(void)
{
    int dtState, clkState;
    unsigned long mills;

    // constant mills across update() function
    mills = millis();

    // speed calculation
    if (mills - lastSpeedMeasurement >= speedPeriod) {
        // approximate speed
        stepsPerSecond = stepCount * 1000 / speedPeriod;
        // reset stepCount
        stepCount = 0;
        // start new measurement
        lastSpeedMeasurement = mills;
    }

    // get current output
    dtState  = digitalRead(dtPin);
    clkState = digitalRead(clkPin);

    // shift out previous output
    prevCurOutput <<= 2;

    // shift in current output
    prevCurOutput |= dtState  << 1;
    prevCurOutput |= clkState << 0;

    // only look at lower 4 bits
    prevCurOutput &= 0x0F;

    // check if rotation is valid
    if (ROTATION_LOOK_UP[prevCurOutput]) {
        // shift out previous prevCurOutput
        rotationCode <<= 4;
        // shift in current prevCurOutput
        rotationCode |= prevCurOutput;

        if (rotationCode == rotationCodeCW) {
            // clockwise rotation
            stepCount++;
            // since we got one step speed is 1
            if (stepsPerSecond == 0) stepsPerSecond++;
            onStepFunc(ENCODER_CW, stepsPerSecond);
        } else if (rotationCode == rotationCodeCCW) {
            // counter-clockwise rotation
            stepCount++;
            // since we got one step speed is 1
            if (stepsPerSecond == 0) stepsPerSecond++;
            onStepFunc(ENCODER_CCW, stepsPerSecond);
        }
    }
}
