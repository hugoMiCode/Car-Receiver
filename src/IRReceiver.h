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
    int oldState = HIGH;
    int8_t bitBuffer[BUFFER_SIZE]; // changer pour gagner de l'espace memoire -> ie morpion en c E3A
    uint8_t bufferPosition; // position dans le buffer
    Chrono::chrono_t highTime;
    Chrono::chrono_t lowTime;

    Chrono lapClock;
    Chrono sectorClock;

    bool passesSectorOrFinish;


    void readPin();
    int decodeBitPeriode();
    Puce decodePuceBuffer();
    void clearBuffer();

  public:
    IRReceiver(int pin);

    void setup();
    void loop();

    bool received();
    
    int sectorTime;
    Puce puceIdPassed;
};

#endif // IR_RECEIVER_H
