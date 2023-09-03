#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <Arduino.h>
#include <Chrono.h>

#define BUFFER_SIZE 6
#define MIN_SECTOR_TIME_MS 1000 // trop court il faudra le modifier


enum class Puce{
  None = 0,
  Finish = 1,
  Sector1 = 2,
  Sector2 = 3
};

class IRReceiver {
  private:
    int irPin;
    
    Chrono pulseClock;
    int oldState;
    uint8_t dataBuffer[BUFFER_SIZE];
    uint8_t cursor; // position dans le buffer
    int highTime;
    int lowTime;

    Chrono lapClock;

    int decodeBitPeriode();
    Puce decodePuceBuffer();
    void clearBuffer();

  public:
    IRReceiver(int pin);

    void setup();
    void loop();
};

#endif // IR_RECEIVER_H
