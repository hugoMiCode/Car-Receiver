#include "IRReceiver.h"


IRReceiver::IRReceiver(int pin) {
  this->irPin = pin;
}

//decode le signal et stock dans le buffer
void IRReceiver::readPin() {
  if (oldState != digitalRead(irPin)) {
    if (oldState == LOW) {
      oldState = HIGH;

      highTime = pulseClock.elapsed();
      pulseClock.restart();
    }
    else {
      oldState = LOW;

      lowTime = pulseClock.elapsed();
      pulseClock.restart();

      bitBuffer[bufferPosition] = decodeBitPeriode();
      bufferPosition = (bufferPosition + 1) % BUFFER_SIZE;
    }
  }
}

int IRReceiver::decodeBitPeriode() {
  if (abs((int32_t)lowTime - 400) < 150)
    return 0;
  if (abs((int32_t)lowTime - 800) < 150)
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

  pulseClock = Chrono(Chrono::MICROS);
  finishClock = Chrono(Chrono::MILLIS);
  sectorClock = Chrono(Chrono::MILLIS);

  clearBuffer();

  pucePassed = Puce::None;
}

void IRReceiver::loop() {
  if (sectorClock.hasPassed(MIN_SECTOR_TIME_MS, false)) {

    readPin();

    Puce puceIdPassed = decodePuceBuffer();
    
      
    switch (puceIdPassed)
    {
    case Puce::None:
      break;
    case Puce::Finish:
      sectorTime = finishClock.elapsed();
      finishClock.restart();
      sectorClock.restart();
      pucePassed = Puce::Finish;
      triggerSendTimeSector = true;
      clearBuffer();

      Serial.println("Puce: Finish, temps: " + String(sectorTime));

     break;
    case Puce::Sector1:
      sectorTime = sectorClock.elapsed();
      sectorClock.restart();
      pucePassed = Puce::Sector1;
      triggerSendTimeSector = true;
      clearBuffer();

      Serial.println("Puce: Sector1, temps: " + String(sectorTime));
      break;
    case Puce::Sector2:
      sectorTime = sectorClock.elapsed();
      sectorClock.restart();
      pucePassed = Puce::Sector2;
      triggerSendTimeSector = true;
      clearBuffer();

      Serial.println("Puce: Sector2, temps: " + String(sectorTime));
      break;
    default:
      break;
    }
  }
    // stocker l'info dans la class, l'acceder avec un geteur => envoyer dans main avec la class wifisender 
}

bool IRReceiver::puceDetected()
{
  if (triggerSendTimeSector) {
      triggerSendTimeSector = false;
    return true;
  }

  return false;
}

Puce IRReceiver::getPucePassed()
{
  return pucePassed;
}

Chrono::chrono_t IRReceiver::getSectorTime()
{
  return sectorTime;
}
