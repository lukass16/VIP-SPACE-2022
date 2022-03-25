#include <Arduino.h>
#include "lora_rswrapper.h"

void setup() {
  Serial.begin(115200);
  lora::setup();
}

void loop() {
  lora::readMessage();
}