#include "IRReceiver.h"


IRReceiver::IRReceiver() {}

int IRReceiver::decodeBitPeriode() {
  if (abs(lowTime - 400) < 150)
    return 0;
  if (abs(lowTime - 800) < 150)
    return 1;
  return -1;
}

Puce IRReceiver::decodePuceBuffer() {
  bool puce1 = true;
  bool puce2 = true;
  bool puce3 = true;
  
  int oldBit = -1;
  bool falseBit = false;
  
  for (int i = 0; i < BUFFER_SIZE; i++) {
    int newBit = dataBuffer[(cursor + i) % BUFFER_SIZE];

    if (newBit == -1) {
      if (falseBit)
        return Puce::None;

      falseBit = true;
      continue;    
    }
          
    if (newBit == 0)
      puce2 = false;
    else if (newBit == 1)
      puce1 = false;

    if (newBit == oldBit)
      puce3 = false;

    oldBit = newBit;
  }
  
  if (puce1)
    return Puce::Finish;
  if (puce2)
    return Puce::Sector1;
  if (puce3)
    return Puce::Sector2;

  return Puce::None;
}

void IRReceiver::clearBuffer() {
  for (int i = 0; i < BUFFER_SIZE; i++) {
    dataBuffer[i] = -1;
    cursor = 0;
  }  
}

void IRReceiver::setup() {
  pinMode(IR_RECEIVE_PIN, INPUT);

  lapClock = Chrono(Chrono::MILLIS);
}

void IRReceiver::loop() {
  //decode le signal et stock dans le buffer
  if (oldState != digitalRead(IR_RECEIVE_PIN)) {
    if (pulseClock.hasPassed(100, false)) {
      if (oldState == LOW) {
        oldState = HIGH;
        highTime = pulseClock.elapsed();
      }
      else {
        oldState = LOW;
        lowTime = pulseClock.elapsed();
            
        dataBuffer[cursor] = decodeBitPeriode();

        cursor = (cursor + 1) % BUFFER_SIZE;
      }
    }
    pulseClock.restart();
  }


  
  if (lapClock.hasPassed(MIN_SECTOR_TIME_MS, false)) {
    Puce puce = decodePuceBuffer();
    
    switch (puce)
    {
    case Puce::None:
      break;
    case Puce::Sector1:
      break;
    case Puce::Sector2:
      break;
    case Puce::Finish:
      lapClock.restart();
      break;
    default:
      break;
    }

    // stocker l'info dans la class, l'acceder avec un geteur => envoyer dans main avec la class wifisender 
  }
}
