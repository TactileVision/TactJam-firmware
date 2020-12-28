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
  delay(2000);
  display.DrawMenuScreen();
  delay(1000);
  display.DrawContentTeaser("TactJam");
  delay(1000);
  display.DrawTactonDetails(1, "0xF00", 42, 20000U);
}


void loop() {
  delay(1000);
  static uint32_t i = 0;
  display.DrawAmplitude(++i%256);
  display.DrawSlotSelection(i%3);
  display.DrawModeSelection((i%10 == 0) ? "jam" : "rec");
}
