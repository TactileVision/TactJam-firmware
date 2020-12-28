#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include "config.h"
#include "display.h"
#include "linearEncoder.h"
#include "rotarySwitch3Pos.h"
#include "mode.h"


tact::Display display;
tact::LinearEncoder amplitude_encoder(tact::config::ESP_pin_linear_encoder);
tact::RotarySwitch3Pos mode_encoder(tact::config::ESP_pin_mode_encoder);
tact::RotarySwitch3Pos slot_encoder(tact::config::ESP_pin_slot_encoder);


void setup() {
  Serial.begin(tact::config::serial_baud_rate);
  while (!Serial) {
    delay(5);
  }

  #ifdef TACT_DEBUG
  Serial.printf("TactJam (TACT v%s-%s)\n", TACT_VERSION, GIT_REV);
  #endif //TACT_DEBUG

  amplitude_encoder.Initialize();
  delay(tact::config::initialization_delay);
  mode_encoder.Initialize();
  delay(tact::config::initialization_delay);
  slot_encoder.Initialize();
  delay(tact::config::initialization_delay);

  if (!display.Initialize()) {
    #ifdef TACT_DEBUG
    Serial.println("ERROR: display setup");
    #endif //TACT_DEBUG
  }
  display.DrawBootScreen();
  delay(2000);
  display.DrawMenuScreen(
    tact::Mode::GetName(static_cast<tact::Modes>(mode_encoder.GetPosition())),
    slot_encoder.GetPosition(),
    amplitude_encoder.GetPercent()
  );
  delay(1000);
}


void loop() {
  if (amplitude_encoder.UpdateAvailable()) {
    display.DrawAmplitude(amplitude_encoder.GetPercent());
  }

  if (mode_encoder.UpdateAvailable()) {
    auto mode_text = tact::Mode::GetName(static_cast<tact::Modes>(mode_encoder.GetPosition()));
    display.DrawModeSelection(mode_text);
  }

  if (slot_encoder.UpdateAvailable()) {
    display.DrawSlotSelection(slot_encoder.GetPosition());
  }

  delay(20);
}
