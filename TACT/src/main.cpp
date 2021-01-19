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
#include "buzzer.h"
#include "tactons.h"

tact::State current_state;
tact::State previous_state;
tact::Display display;
tact::LinearEncoder amplitude_encoder(tact::config::esp::pins::kLinearEncoder);
tact::RotarySwitch3Pos mode_encoder(tact::config::esp::pins::kModeEncoder);
tact::RotarySwitch3Pos slot_encoder(tact::config::esp::pins::kSlotEncoder);
tact::M74HC166 buttons(
  tact::config::esp::pins::kM74HC166Latch,
  tact::config::esp::pins::kM74HC166Clock,
  tact::config::esp::pins::kM74HC166Data
);
tact::SN74HC595 button_leds(
  tact::config::esp::pins::kSN74HC595Latch,
  tact::config::esp::pins::kSN74HC595Clock,
  tact::config::esp::pins::kSN74HC595Data
);
tact::PCA9685 actuator_driver;
#ifdef __TACT_BUZZER_MULTIPLEXER__
tact::Buzzer buzzer;
#else
tact::Buzzer buzzer(tact::config::esp::pins::kBuzzer);
#endif
tact::TactonRecorderPlayer tactonRecorderPlayer(&display, &actuator_driver, &button_leds);


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
  Serial.begin(tact::config::kSerialBaudRate);
  while (!Serial) {
    delay(5);
  }

  #ifdef TACT_DEBUG
  Serial.printf("TactJam (TACT %s-%s)\n", GIT_TAG, GIT_REV);
  #endif //TACT_DEBUG

  amplitude_encoder.Initialize();
  current_state.amplitude = amplitude_encoder.Get12bit();
  current_state.amplitude_percent = amplitude_encoder.GetPercent();
  delay(tact::config::kInitializationDelay);
  mode_encoder.Initialize();
  current_state.mode = static_cast<tact::Modes>(mode_encoder.GetPosition());
  delay(tact::config::kInitializationDelay);
  slot_encoder.Initialize();
  current_state.slot = slot_encoder.GetPosition();
  delay(tact::config::kInitializationDelay);
  buttons.Initialize();
  delay(tact::config::kInitializationDelay);
  button_leds.Initialize();
  delay(tact::config::kInitializationDelay);
  button_leds.Update(0);
  delay(tact::config::kInitializationDelay);
  actuator_driver.Initialize();
  delay(tact::config::kInitializationDelay);
  actuator_driver.Update(0, 0);
  delay(tact::config::kInitializationDelay);
  buzzer.Initialize();
  delay(tact::config::kInitializationDelay);

  if (!display.Initialize()) {
    #ifdef TACT_DEBUG
    Serial.println("ERROR: display setup");
    #endif //TACT_DEBUG
  }
  
  display.DrawBootScreen();
  buzzer.PlayInitSequence();
  delay(2000);

  display.DrawMenuScreen(
    tact::Mode::GetName(static_cast<tact::Modes>(mode_encoder.GetPosition())),
    slot_encoder.GetPosition(),
    amplitude_encoder.GetPercent()
  );
  delay(1000);
  
  previous_state = current_state;

  tact::config::esp::DisableRadios();
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
    if (tact::config::kDebugLevel == tact::config::DebugLevel::verbose) {
      Serial.printf("slot: %u\n", current_state.slot);
    }
    #endif //TACT_DEBUG
  }

  if (amplitude_encoder.UpdateAvailable()) {
    current_state.amplitude = amplitude_encoder.Get12bit();
    current_state.amplitude_percent = amplitude_encoder.GetPercent();
    display.DrawAmplitude(current_state.amplitude_percent);
    #ifdef TACT_DEBUG
    if (tact::config::kDebugLevel == tact::config::DebugLevel::verbose) {
      Serial.printf("amplitude: %u(12bit) %u(percent)\n", current_state.amplitude, current_state.amplitude_percent);
    }
    #endif //TACT_DEBUG
  }

  if (current_state.slot != previous_state.slot ||
    current_state.mode != previous_state.mode) {
    actuator_driver.Update(0, 0);
    button_leds.Update(0);
    display.ClearContentTeaser();
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

void ReadButtons() {
  if (buttons.UpdateAvailable()) {
    current_state.pressed_buttons = buttons.Read();
    current_state.pressed_actuator_buttons = current_state.pressed_buttons >> 8u;
    current_state.pressed_menu_buttons = (current_state.pressed_buttons >> 5u) & 0x7u;
  
    #ifdef TACT_DEBUG
    if (tact::config::kDebugLevel == tact::config::DebugLevel::verbose) {
      if (previous_state.pressed_actuator_buttons != current_state.pressed_actuator_buttons) {
        Serial.print("active actuator buttons BIN: ");
        Serial.print(current_state.pressed_actuator_buttons, BIN);
        Serial.printf("\t amplitude: %u(12bit) %u(percent)\n", current_state.amplitude, current_state.amplitude_percent);
      }
  
      if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
        Serial.print("active menu buttons BIN: ");
        Serial.println(current_state.pressed_menu_buttons, BIN);
      }
    }
    #endif //TACT_DEBUG
  }
}

void HandleJamMode() {
  ReadButtons();
  if (previous_state.pressed_actuator_buttons != current_state.pressed_actuator_buttons) {
    button_leds.Update(current_state.pressed_actuator_buttons);
    actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
    current_state.pressed_actuator_buttons = current_state.pressed_actuator_buttons;
  }

  if (previous_state.amplitude_percent != current_state.amplitude_percent) {
    actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
  }

  // TODO: check if delay is needed
  delay(2);
}


void HandleRecPlayMode() {
  // TODO: amplitude modulation after recording (#10): https://github.com/TactileVision/TactJam-firmware/issues/10
  // TODO: delete current tacton (#12): https://github.com/TactileVision/TactJam-firmware/issues/12
  ReadButtons();

  if (current_state.slot != previous_state.slot ||
    current_state.mode != previous_state.mode) {
    tactonRecorderPlayer.Reset();
  }

  if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
    if ( (previous_state.pressed_menu_buttons & 4) == 4) {
      //menu button 1 pressed, start record
      tactonRecorderPlayer.RecordButtonPressed(current_state, buzzer);
    }
    if ( (previous_state.pressed_menu_buttons & 2) == 2) {
      //menu button 1 pressed, start record
      tactonRecorderPlayer.PlayButtonPressed(buzzer);
    }
    if ( (previous_state.pressed_menu_buttons & 1) == 1) {
      //menu button 3 pressed, switch loop
      tactonRecorderPlayer.LoopButtonPressed(buzzer);
    }
  }

  if (previous_state.pressed_actuator_buttons != current_state.pressed_actuator_buttons) {
    tactonRecorderPlayer.RecordSample(current_state, buzzer);
  }
  if (previous_state.amplitude != current_state.amplitude &&
    current_state.pressed_actuator_buttons != 0) {
    tactonRecorderPlayer.RecordSample(current_state, buzzer);
  }
  tactonRecorderPlayer.PlaySample(current_state, buzzer, amplitude_encoder);


  //#ifdef TACT_DEBUG
  //Serial.println("Record and Play Mode is not implemented yet");
  //#endif //TACT_DEBUG

  // TODO: check if delay is needed
  delay(2);
}


void HandleDataTransferMode() {
  // TODO: receive tacton from PC (#7): https://github.com/TactileVision/TactJam-firmware/issues/7
  // TODO: send tacton to PC (#8): https://github.com/TactileVision/TactJam-firmware/issues/8
  ReadButtons();
  if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
    if ( (previous_state.pressed_menu_buttons & 4) == 4) {
      //menu button 1 pressed
      buzzer.PlayConfirm();
      tactonRecorderPlayer.ToVTP(current_state.slot);
      buzzer.PlayConfirm();
    }
  }
  #ifdef TACT_DEBUG
  //Serial.println("Transfer Mode is not implemented yet");
  #endif //TACT_DEBUG
  delay(2);
}
