#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include "config.h"
#include "display.h"
#include "linearEncoder.h"
#include "rotarySwitch3Pos.h"
#include "mode.h"
#include "M74HC166.h"


tact::Display display;
tact::LinearEncoder amplitude_encoder(tact::config::ESP_pin_linear_encoder);
tact::RotarySwitch3Pos mode_encoder(tact::config::ESP_pin_mode_encoder);
tact::RotarySwitch3Pos slot_encoder(tact::config::ESP_pin_slot_encoder);
tact::M74HC166 buttons(
  tact::config::ESP_pin_M74HC166_latch,
  tact::config::ESP_pin_M74HC166_clock,
  tact::config::ESP_pin_M74HC166_data
);


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

  buttons.Initialize();

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
  auto pressed_buttons = buttons.Read();
  auto pressed_actuator_buttons = pressed_buttons >> 8;
  if (pressed_actuator_buttons != 0) {
    #ifdef TACT_DEBUG
    Serial.print("activeActuatorButtons BIN: ");
    Serial.println(pressed_actuator_buttons, BIN);
    #endif //TACT_DEBUG
  }
  uint8_t pressed_menu_buttons = (pressed_buttons >> 5) & 0x7;
  if (pressed_menu_buttons != 0) {
    #ifdef TACT_DEBUG
    Serial.print("activeMenuButtons BIN: ");
    Serial.println(pressed_menu_buttons, BIN);
    #endif //TACT_DEBUG
  }

  if (amplitude_encoder.UpdateAvailable()) {
    display.DrawAmplitude(amplitude_encoder.GetPercent());
  }

  if (mode_encoder.UpdateAvailable()) {
    auto mode_text = tact::Mode::GetName(static_cast<tact::Modes>(mode_encoder.GetPosition()));
    display.DrawModeSelection(mode_text);
    // [TODO] change setup
  }

  if (slot_encoder.UpdateAvailable()) {
    display.DrawSlotSelection(slot_encoder.GetPosition());
    // [TODO] change setup
  }

  delay(20);
}
