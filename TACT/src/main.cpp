#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include "config.h"
#include "mode.h"
#include "state.h"
#include "display.h"
#include "linearEncoder.h"
#include "rotarySwitch3Pos.h"
#include "M74HC166.h"
#include "SN74HC595.h"
#include "PCA9685.h"

tact::State current_state;
tact::State previous_state;
tact::Display display;
tact::LinearEncoder amplitude_encoder(tact::config::ESP_pin_linear_encoder);
tact::RotarySwitch3Pos mode_encoder(tact::config::ESP_pin_mode_encoder);
tact::RotarySwitch3Pos slot_encoder(tact::config::ESP_pin_slot_encoder);
tact::M74HC166 buttons(
  tact::config::ESP_pin_M74HC166_latch,
  tact::config::ESP_pin_M74HC166_clock,
  tact::config::ESP_pin_M74HC166_data
);
tact::SN74HC595 button_leds(
  tact::config::ESP_pin_SN74HC595_latch,
  tact::config::ESP_pin_SN74HC595_clock,
  tact::config::ESP_pin_SN74HC595_data
);
tact::PCA9685 actuator_driver;

/**
 * @brief 
 * 
 */
void HandleJamMode();

/**
 * @brief 
 * 
 */
void HandleRecPlayMode();

/**
 * @brief 
 * 
 */
void HandleDataTransferMode();


void setup() {
  Serial.begin(tact::config::serial_baud_rate);
  while (!Serial) {
    delay(5);
  }

  #ifdef TACT_DEBUG
  Serial.printf("TactJam (TACT %s-%s)\n", TACT_VERSION, GIT_REV);
  #endif //TACT_DEBUG

  amplitude_encoder.Initialize();
  current_state.amplitude = amplitude_encoder.Get12bit();
  current_state.amplitude_percent = amplitude_encoder.GetPercent();
  delay(tact::config::initialization_delay);
  mode_encoder.Initialize();
  current_state.mode = static_cast<tact::Modes>(mode_encoder.GetPosition());
  delay(tact::config::initialization_delay);
  slot_encoder.Initialize();
  current_state.slot = slot_encoder.GetPosition();
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
  previous_state = current_state;
}


void loop() {
  if (mode_encoder.UpdateAvailable()) {
    current_state.mode = static_cast<tact::Modes>(mode_encoder.GetPosition());
    auto mode_text = tact::Mode::GetName(current_state.mode);
    display.DrawModeSelection(mode_text);
  }

  if (slot_encoder.UpdateAvailable()) {
    // TODO: swop current tacton (#6): https://github.com/TactileVision/TactJam-firmware/issues/6
    current_state.slot = slot_encoder.GetPosition();
    display.DrawSlotSelection(current_state.slot);
    #ifdef TACT_DEBUG
    if (tact::config::debug_level == tact::config::DebugLevel::verbose) {
      Serial.printf("slot: %u\n", current_state.slot);
    }
    #endif //TACT_DEBUG
  }

  if (amplitude_encoder.UpdateAvailable()) {
    current_state.amplitude = amplitude_encoder.Get12bit();
    current_state.amplitude_percent = amplitude_encoder.GetPercent();
    display.DrawAmplitude(current_state.amplitude_percent);
    #ifdef TACT_DEBUG
    if (tact::config::debug_level == tact::config::DebugLevel::verbose) {
      Serial.printf("amplitude: %u(12bit) %u(percent)\n", current_state.amplitude, current_state.amplitude_percent);
    }
    #endif //TACT_DEBUG
  }

  switch (current_state.mode) {
    case tact::Modes::undefined :
      #ifdef TACT_DEBUG
      Serial.println("device is in an undefined mode");
      #endif //TACT_DEBUG
      break;
    case tact::Modes::jam :
      HandleJamMode();
      break;
    case tact::Modes::rec_play :
      HandleRecPlayMode();
      break;
    case tact::Modes::transfer :
      HandleDataTransferMode();
      break;
  }

  previous_state = current_state;
}


void HandleJamMode() {
  if (buttons.UpdateAvailable()) {
    current_state.pressed_buttons = buttons.Read();
    current_state.pressed_actuator_buttons = current_state.pressed_buttons >> 8u;
    current_state.pressed_menu_buttons = (current_state.pressed_buttons >> 5u) & 0x7u;
  }

  if (previous_state.pressed_actuator_buttons != current_state.pressed_actuator_buttons) {
    button_leds.Update(current_state.pressed_actuator_buttons);
    actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
    current_state.pressed_actuator_buttons = current_state.pressed_actuator_buttons;
    #ifdef TACT_DEBUG
    if (tact::config::debug_level == tact::config::DebugLevel::verbose) {
      Serial.print("active actuator buttons BIN: ");
      Serial.print(current_state.pressed_actuator_buttons, BIN);
      Serial.printf("\t amplitude: %u(12bit) %u(percent)\n", current_state.amplitude, current_state.amplitude_percent);
    }
    #endif //TACT_DEBUG
  }

  if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
    #ifdef TACT_DEBUG
    if (tact::config::debug_level == tact::config::DebugLevel::verbose) {
      Serial.print("active menu buttons BIN: ");
      Serial.println(current_state.pressed_menu_buttons, BIN);
    }
    #endif //TACT_DEBUG
  }

  if (previous_state.amplitude_percent != current_state.amplitude_percent) {
    actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
  }

  delay(2);
}


void HandleRecPlayMode() {
  // TODO: play selected tacton (#5): https://github.com/TactileVision/TactJam-firmware/issues/5
      // TODO: toggle loop mode (#9): https://github.com/TactileVision/TactJam-firmware/issues/9
  // TODO: record tacton (#3): https://github.com/TactileVision/TactJam-firmware/issues/3
      // TODO: amplitude modulation after recording (#10): https://github.com/TactileVision/TactJam-firmware/issues/10
  // TODO: delete current tacton (#12): https://github.com/TactileVision/TactJam-firmware/issues/12
  #ifdef TACT_DEBUG
  Serial.println("Record and Play Mode is not implemented yet");
  #endif //TACT_DEBUG
  delay(2000);
}


void HandleDataTransferMode() {
  // TODO: receive tacton from PC (#7): https://github.com/TactileVision/TactJam-firmware/issues/7
  // TODO: send tacton to PC (#8): https://github.com/TactileVision/TactJam-firmware/issues/8
  #ifdef TACT_DEBUG
  Serial.println("Transfer Mode is not implemented yet");
  #endif //TACT_DEBUG
  delay(2000);
}
