#ifndef WIFI_SENDER_H
#define WIFI_SENDER_H

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>


class WifiSender {
private:
  RF24 radio;
  const byte address[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

public:
  WifiSender(uint8_t cePin, uint8_t csnPin) : radio(cePin, csnPin) {}

  void setup() {
    radio.begin();
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening();
  }

  bool available() {
    return radio.available();
  }

  void sendMessage(const char* message) {
    radio.write(message, strlen(message) + 1);
  }
};

#endif // WIFI_SENDER_H
