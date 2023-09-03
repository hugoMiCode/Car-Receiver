#include "IRReceiver.h"


IRReceiver::IRReceiver(int pin) {
  this->irPin = pin;
}

int IRReceiver::decodeBitPeriode() {
  if (abs(lowTime - 400) < 150)
    return 0;
  if (abs(lowTime - 800) < 150)
    return 1;
  return -1;
}

Puce IRReceiver::decodePuceBuffer() {
  bool finish = true;
  bool sector1 = true;
  bool sector2 = true;
  
  int oldBit = -1;
  bool falseBit = false;
  
  for (int i = 0; i < BUFFER_SIZE; i++) {
    int newBit = bitBuffer[(bufferPosition + i) % BUFFER_SIZE];

    if (newBit == -1) {
      if (falseBit)
        return Puce::None;

      falseBit = true;
      continue;    
    }
          
    if (newBit == 0)
      sector1 = false;
    else if (newBit == 1)
      finish = false;

    if (newBit == oldBit)
      sector2 = false;

    oldBit = newBit;
  }
  
  if (finish)
    return Puce::Finish;
  if (sector1)
    return Puce::Sector1;
  if (sector2)
    return Puce::Sector2;

  return Puce::None;
}

void IRReceiver::clearBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    bitBuffer[i] = -1;
    bufferPosition = 0;
  }  
}

void IRReceiver::setup() {
  pinMode(irPin, INPUT);

  lapClock = Chrono(Chrono::MILLIS);
  sectorClock = Chrono(Chrono::MILLIS);
}

void IRReceiver::loop() {
  //decode le signal et stock dans le buffer
  if (oldState != digitalRead(irPin)) {
    if (pulseClock.hasPassed(100, false)) {
      if (oldState == LOW) {
        oldState = HIGH;
        highTime = pulseClock.elapsed();
      }
      else {
        oldState = LOW;
        lowTime = pulseClock.elapsed();
            
        bitBuffer[bufferPosition] = decodeBitPeriode();

        bufferPosition = (bufferPosition + 1) % BUFFER_SIZE;
      }
    }
    pulseClock.restart();
  }


  
  if (sectorClock.hasPassed(MIN_SECTOR_TIME_MS, false)) {
    puceIdPassed = decodePuceBuffer();
    
    switch (puceIdPassed)
    {
    case Puce::None:
      break;
    case Puce::Sector1:
      sectorTime = sectorClock.elapsed();
      sectorClock.restart();
      passesSectorOrFinish = true;
      break;
    case Puce::Sector2:
      sectorTime = sectorClock.elapsed();
      sectorClock.restart();
      passesSectorOrFinish = true;
      break;
    case Puce::Finish:
      sectorTime = lapClock.elapsed();
      lapClock.restart();
      passesSectorOrFinish = true;
      break;
    default:
      break;
    }

    // stocker l'info dans la class, l'acceder avec un geteur => envoyer dans main avec la class wifisender 
  }
}

bool IRReceiver::received()
{
  if (passesSectorOrFinish) {
    passesSectorOrFinish = false;
    return true;
  }

  return false;
}
