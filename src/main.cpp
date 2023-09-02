#include <Chrono.h>
#include "WifiSender.h"
#include "IRReceiver.h"



IRReceiver irReceiver;

void setup() {
  Serial.begin(115200);

  irReceiver.setup();
}

void loop() { 
  irReceiver.loop();
}
