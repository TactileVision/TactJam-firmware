#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include "config.h"
#include "display.h"


tact::Display display;


void setup() {
  Serial.begin(tact::config::serial_baud_rate);
  while (!Serial) {
    delay(5);
  }

  #ifdef TACT_DEBUG
  Serial.printf("TactJam (TACT v%s-%s)\n", TACT_VERSION, GIT_REV);
  #endif //TACT_DEBUG

  if (!display.Initialize()) {
    #ifdef TACT_DEBUG
    Serial.println("ERROR: display setup");
    #endif //TACT_DEBUG
  }
  display.DrawBootScreen();
  delay(4000);
  display.DrawMenuScreen();
}


void loop() {
  delay(3000);
  static uint32_t i = 0;
  display.UpdateAmplitude(++i%256);
  display.UpdateSlotSelection(i%3);
  display.UpdateModeSelection((i%10 == 0) ? "jam" : "rec");
}
