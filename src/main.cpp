#include <Chrono.h>
#include "WifiSender.h"
#include "IRReceiver.h"

#define IR_RECEIVER_PIN 2
#define WIFI_CE_PIN 7
#define WIFI_CSN_PIN 8

IRReceiver irReceiver(IR_RECEIVER_PIN);
WifiSender wifiSender(WIFI_CE_PIN, WIFI_CSN_PIN);

void setup() {
  Serial.begin(115200);

  irReceiver.setup();
  wifiSender.setup();
}

void loop() { 
  irReceiver.loop();// pull puce detected => envoie avec wifiSender class

  
  const char* message = "Mon message";
  wifiSender.sendMessage(message);
}
