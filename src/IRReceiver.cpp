#include "IRReceiver.h"


IRReceiver::IRReceiver(int pin) {
  this->irPin = pin;
  pulseClock = Chrono(Chrono::MICROS);
  finishClock = Chrono(Chrono::MILLIS);
  sectorClock = Chrono(Chrono::MILLIS);

  oldState = HIGH;
  pucePassed = Puce::None;

  pinMode(pin, INPUT);
}

//On pourrait faire un par un plutôt qu'attendre d'avoir le signal LOW => gagner du temps mieux repartie
void IRReceiver::readPin() {
  if (oldState != digitalRead(irPin)) {
    if (oldState == LOW) {
      oldState = HIGH;

      highTime = pulseClock.elapsed();
      pulseClock.restart();

      decodeBitHigh();
    }
    else {
      oldState = LOW;

      lowTime = pulseClock.elapsed();
      pulseClock.restart();

      decodeBitLow();
    }
  }
}


void IRReceiver::decodeBitHigh() {
  if (abs((int32_t)highTime - HIGH_SHORT_TIME) < 100) {
    validBuffer |= bufferPosition; // on met le bit à 1
    
    if (HIGH_SHORT_BIT)
      signalBuffer |= bufferPosition; // on met le bit à 1
    else 
      signalBuffer &= ~bufferPosition; // on met le bit à 0
  }
  else if (abs((int32_t)highTime - HIGH_LONG_TIME) < 100) {
    validBuffer |= bufferPosition;
    
    if (HIGH_LONG_BIT)
      signalBuffer |= bufferPosition; // on met le bit à 1
    else 
      signalBuffer &= ~bufferPosition; // on met le bit à 0    
  }
  else {
    // bit non valide
    validBuffer &= ~bufferPosition; // on met le bit à 0
  }

  bufferPosition <<= 1;
}

void IRReceiver::decodeBitLow() {
  if (abs((int32_t)lowTime - LOW_SHORT_TIME) < 100) {
    validBuffer |= bufferPosition;

    if (LOW_SHORT_BIT)
      signalBuffer |= bufferPosition; // on met le bit à 1
    else 
      signalBuffer &= ~bufferPosition; // on met le bit à 0
  }
  else if (abs((int32_t)lowTime - LOW_LONG_TIME) < 100) {
    validBuffer |= bufferPosition;

    if (LOW_LONG_BIT)
      signalBuffer |= bufferPosition; // on met le bit à 1
    else 
      signalBuffer &= ~bufferPosition; // on met le bit à 0
  }
  else {
    // bit non valide
    validBuffer &= ~bufferPosition; // on met le bit à 0
  }

  bufferPosition <<= 1;

  if (bufferPosition == 0b0)
    bufferPosition = 0b1;
}

Puce IRReceiver::decodePuceBuffer() {

  if (validBuffer != 0b11111111)
    return Puce::None;

  const uint8_t maskFinish = 0b00000000;
  const uint8_t maskSector1 = 0b10101010;
  const uint8_t maskSector2 = 0b01010101;
  const uint8_t maskSector3 = 0b11111111;

  switch (signalBuffer)
  {
  case maskFinish:
    return Puce::Finish;
    break;
  case maskSector1:
    return Puce::Sector1;
    break;
  case maskSector2:
    return Puce::Sector2;
    break;
  case maskSector3:
    return Puce::Sector3;
    break;
  default:
    break;
  }

  return Puce::None;
}

void IRReceiver::clearBuffer() {
  signalBuffer = 0;
  validBuffer = 0;
  bufferPosition = 0;
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
    case Puce::Sector3:
      sectorTime = sectorClock.elapsed();
      sectorClock.restart();
      pucePassed = Puce::Sector3;
      triggerSendTimeSector = true;
      clearBuffer();

      Serial.println("Puce: Sector3, temps: " + String(sectorTime));
      break;
    default:
      break;
    }
  }
}

bool IRReceiver::puceDetected() {
  if (triggerSendTimeSector) {
      triggerSendTimeSector = false;
    return true;
  }

  return false;
}

Puce IRReceiver::getPucePassed() {
  return pucePassed;
}

Chrono::chrono_t IRReceiver::getSectorTime() {
  return sectorTime;
}
