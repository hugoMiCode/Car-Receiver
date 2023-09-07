#include <Chrono.h>
#include "WifiSender.h"
#include "IRReceiver.h"

#define IR_RECEIVER_PIN 2
#define WIFI_CE_PIN 10
#define WIFI_CSN_PIN 9

IRReceiver irReceiver(IR_RECEIVER_PIN);
WifiSender wifiSender(WIFI_CE_PIN, WIFI_CSN_PIN);

void setup() {
  Serial.begin(115200);

  irReceiver.setup();
  wifiSender.setup();
}

void loop() { 
  irReceiver.loop();

  if (irReceiver.puceDetected()) {

    // Il faudra creer un protocole pour communiquer les messages en moins de 32bits
    String message = String(int(irReceiver.getPucePassed())) + "," + String(irReceiver.getSectorTime());
    Serial.println("Sending message: " + message);
    wifiSender.sendMessage(&message[0]);

    delay(1000);
  }
}
