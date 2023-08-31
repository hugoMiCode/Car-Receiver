#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <Arduino.h>
#include <Chrono.h>

const int bufferSize = 6;


class IRReceiver {
  private:
    Chrono pulseClock;
    int oldState;
    bool cleared;
    uint8_t dataBuffer[bufferSize];
    uint8_t cursor;
    int highTime;
    int lowTime;
    Chrono detectionGapClock;
    bool canBeDetected;
    Chrono lapClocks[3];

    int decodeBitPeriode();
    int decodePuceBuffer();
    void clearBuffer();

  public:
    IRReceiver();

    void setup();
    void loop();
};

#endif // IR_RECEIVER_H
