#include "IRReceiver.h"

const int IR_RECEIVE_PIN = 2;
const uint64_t minTimeLapMs = 500;

IRReceiver::IRReceiver() {}

int IRReceiver::decodeBitPeriode() {
    if (abs(lowTime - 400) < 150)
    return 0;
  if (abs(lowTime - 800) < 150)
    return 1;
  return -1;
}

int IRReceiver::decodePuceBuffer() {
  bool puce1 = true;
  bool puce2 = true;
  bool puce3 = true;
  
  int oldBit = -1;
  bool falseBit = false;
  
  for (int i = 0; i < bufferSize; i++) {
    int newBit = dataBuffer[(cursor + i) % bufferSize];

    if (newBit == -1) {
      if (falseBit)
        return 0;

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
    return 1;
  if (puce2)
    return 2;
  if (puce3)
    return 3;

  return 0;}

void IRReceiver::clearBuffer() {
  for (int i = 0; i < bufferSize; i++) {
    dataBuffer[i] = -1;
    cursor = 0;
  }  }

void IRReceiver::setup() {
  pinMode(IR_RECEIVE_PIN, INPUT);
  for (int i = 0; i < 3; i++)
    lapClocks[i] = Chrono(Chrono::MILLIS);
}

void IRReceiver::loop() {
  if (oldState != digitalRead(IR_RECEIVE_PIN)) {
    if (pulseClock.hasPassed(100, false)) {
      if (oldState == LOW) {
        oldState = HIGH;
        highTime = pulseClock.elapsed();
      }
      else {
        oldState = LOW;
        lowTime = pulseClock.elapsed();
            
        dataBuffer[cursor % bufferSize] = decodeBitPeriode();
        //Serial.println(dataBuffer[cursor % bufferSize]);
        cursor++;
      }

      cleared = false;
    }
    pulseClock.restart();
  }

  if (detectionGapClock.hasPassed(300, false)) {
    canBeDetected = true;
  }

  
  if (!cleared && pulseClock.hasPassed(10000, false)) {
    clearBuffer();
    cleared = true;
    canBeDetected = true;
    //Serial.println("Buffer Cleared");
  }  
  
  if (!cleared && canBeDetected) {
    int puceId = decodePuceBuffer();
      
    if (puceId != 0 && lapClocks[puceId - 1].hasPassed(minTimeLapMs, false)) {
      String complete_msg = "{" + String(puceId) + ": " + String(lapClocks[puceId - 1].elapsed()) + "}";
      char *to_send = &complete_msg[0]; 
      //Serial.println("Gap(ms): " + String(detectionGapClock.elapsed()));
      Serial.println(to_send);
    
      lapClocks[puceId - 1].restart();
      detectionGapClock.restart();

      canBeDetected = false;

      digitalWrite(48, HIGH);
      delay(100);
      digitalWrite(48, LOW);
    }
  }
}
