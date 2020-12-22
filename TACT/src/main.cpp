#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include "config.h"

void setup() {
  Serial.begin(tact::config::serial_baud_rate);
  while (!Serial) {
    delay(5);
  }
  #ifdef TACT_DEBUG
  Serial.printf("TactJam (TACT v%s-%s)\n", TACT_VERSION, GIT_REV);
  #endif //TACT_DEBUG
}

void loop() {
  #ifdef TACT_DEBUG
  delay(5000);
  static uint32_t i = 0;
  Serial.printf("TactJam (TACT v%s-%s)\t loop iteration: %u\n", TACT_VERSION, GIT_REV, i++);
  #endif //TACT_DEBUG
}
