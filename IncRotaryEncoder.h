#ifndef _INC_ROTARY_ENCODER_
#define _INC_ROTARY_ENCODER_

#include <Arduino.h>

typedef enum {
    ENCODER_CW,
    ENCODER_CCW,
} IncRotaryEncoderDirection;

/// @brief Function that is called when a step is detected
/// @param direction ENCODER_CW or ENCODER_CCW
/// @param stepsPerSecond absolute speed in steps per second
typedef void (* OnStepFunc)(int direction, int stepsPerSecond);

class IncRotaryEncoder {
public:
    IncRotaryEncoder(uint8_t clkPin, uint8_t dtPin, bool dtIsHighOnClockwise = false);

    void begin(bool useInternalPullup = false);
    void setOnStepFunc(OnStepFunc onStepFunc);
    void setSpeedMeasurementInterval(uint16_t interval);
    void update(void);

private:
    uint8_t clkPin;     // CLK pin on the module
    uint8_t dtPin;      // DT pin on the module

    uint8_t prevCurOutput;      // previous output combined with current output of encoder, CLK pin and DT pin combined (4 Bits)
    uint8_t rotationCode;       // previous output combined with current output (8 Bits)
    uint8_t rotationCodeCW;     // rotation code for clockwise rotation
    uint8_t rotationCodeCCW;    // rotation code for counter clockwise rotation

    OnStepFunc onStepFunc;      // the function that is called when a step is detected

    uint16_t stepCount;                 // steps observed in one period
    uint16_t speedPeriod;               // period for speed measurement
    unsigned long lastSpeedMeasurement; // when the last speed measurement was taken
    int stepsPerSecond;                 // speed in steps per second
};

#endif // _INC_ROTARY_ENCODER_