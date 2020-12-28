#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include "config.h"
#include "display.h"
#include "linearEncoder.h"
#include "rotarySwitch3Pos.h"
#include "mode.h"
#include "M74HC166.h"
#include "SN74HC595.h"
#include "PCA9685.h"


tact::Display display;
tact::LinearEncoder amplitude_encoder(tact::config::ESP_pin_linear_encoder);
tact::RotarySwitch3Pos mode_encoder(tact::config::ESP_pin_mode_encoder);
tact::RotarySwitch3Pos slot_encoder(tact::config::ESP_pin_slot_encoder);
tact::M74HC166 buttons(
  tact::config::ESP_pin_M74HC166_latch,
  tact::config::ESP_pin_M74HC166_clock,
  tact::config::ESP_pin_M74HC166_data
);
uint8_t last_pressed_actuator_buttons = 0;
tact::SN74HC595 button_leds(
  tact::config::ESP_pin_SN74HC595_latch,
  tact::config::ESP_pin_SN74HC595_clock,
  tact::config::ESP_pin_SN74HC595_data
);
tact::PCA9685 actuator_driver;


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
  delay(tact::config::initialization_delay);
  button_leds.Initialize();
  delay(tact::config::initialization_delay);
  button_leds.Update(0);
  delay(tact::config::initialization_delay);
  actuator_driver.Initialize();
  delay(tact::config::initialization_delay);
  actuator_driver.Update(0, 0);
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
  auto pressed_buttons = buttons.Read();
  uint8_t pressed_actuator_buttons = pressed_buttons >> 8u;
  if (pressed_actuator_buttons != last_pressed_actuator_buttons) {
    button_leds.Update(pressed_actuator_buttons);
    auto amplitude = amplitude_encoder.Get12bit();
    actuator_driver.Update(pressed_actuator_buttons, amplitude);
    last_pressed_actuator_buttons = pressed_actuator_buttons;
    #ifdef TACT_DEBUG
    if (tact::config::debug_level == tact::config::DebugLevel::verbose) {
      Serial.print("active actuator buttons BIN: ");
      Serial.println(pressed_actuator_buttons, BIN);
      Serial.printf("amplitude (12bit): %u\n", amplitude);
    }
    #endif //TACT_DEBUG
  }

  uint8_t pressed_menu_buttons = (pressed_buttons >> 5u) & 0x7u;
  if (pressed_menu_buttons != 0) {
    #ifdef TACT_DEBUG
    if (tact::config::debug_level == tact::config::DebugLevel::verbose) {
      Serial.print("active menu buttons BIN: ");
      Serial.println(pressed_menu_buttons, BIN);
    }
    #endif //TACT_DEBUG
  }

  if (amplitude_encoder.UpdateAvailable()) {
    display.DrawAmplitude(amplitude_encoder.GetPercent());
    auto amplitude = amplitude_encoder.Get12bit();
    actuator_driver.Update(pressed_actuator_buttons, amplitude);
    #ifdef TACT_DEBUG
    if (tact::config::debug_level == tact::config::DebugLevel::verbose) {
      Serial.printf("amplitude (12bit): %u\n", amplitude);
    }
    #endif //TACT_DEBUG
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

  // [TODO] replace by timer interrupts
  delay(20);
}
