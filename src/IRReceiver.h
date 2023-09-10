#ifndef IR_RECEIVER_H
#define IR_RECEIVER_H

#include <Arduino.h>
#include <Chrono.h>

#define HIGH_SHORT_TIME 250
#define HIGH_LONG_TIME 500
#define LOW_SHORT_TIME 250
#define LOW_LONG_TIME 500

#define HIGH_SHORT_BIT 0
#define HIGH_LONG_BIT 1 
#define LOW_SHORT_BIT 0
#define LOW_LONG_BIT 1

#define MIN_SECTOR_TIME_MS 1000 // trop court il faudra le modifier


// on pourrait faire 8 secteurs en ajoutant un bit en plus
enum class Puce{
  None = 0b11111111,
  Finish = 0b00,
  Sector1 = 0b01,
  Sector2 = 0b10,
  Sector3 = 0b11
};

class IRReceiver {
  private:
    int irPin;
    
    Chrono pulseClock;
    int oldState;

    uint8_t signalBuffer;
    uint8_t validBuffer;
    uint8_t bufferPosition; // position dans le buffer
    
    Chrono::chrono_t highTime;
    Chrono::chrono_t lowTime;

    Chrono finishClock;
    Chrono sectorClock;

    bool triggerSendTimeSector;
    Puce pucePassed;
    Chrono::chrono_t sectorTime;


    void readPin();
    void decodeBitHigh();
    void decodeBitLow();
    Puce decodePuceBuffer();
    void clearBuffer();

  public:
    IRReceiver(int pin);

    void loop();

    bool puceDetected();
    Puce getPucePassed();
    Chrono::chrono_t getSectorTime();
};

#endif // IR_RECEIVER_H
