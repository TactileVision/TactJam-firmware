#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include <config.h>
#include <mode.h>
#include <state.h>
#include <peripherals.h>

#include <tactons.h>
#include <dataTransfer.h>

tact::Peripherals peripherals;
tact::State current_state;
tact::State previous_state;

tact::TactonRecorderPlayer tacton_recorder_player(&peripherals.display, &peripherals.actuator_driver, &peripherals.button_leds);
tact::DataTransfer data_transfer(&current_state, &peripherals.display, &peripherals.buzzer, &tacton_recorder_player);


void ReadButtons();
void HandleJamMode();
void HandleRecPlayMode();
void HandleDataTransferMode();


void setup() {
  Serial.begin(tact::config::kSerialBaudRate);
  while (!Serial) {
    delay(5);
  }

  #ifdef TACT_DEBUG
  Serial.printf("TactJam (TACT %s-%s)\n", GIT_TAG, GIT_REV);
  #endif //TACT_DEBUG

  tact::config::esp::StartI2C();

  peripherals.Initialize();
  current_state.amplitude = peripherals.amplitude_encoder.Get12bit();
  current_state.amplitude_percent = peripherals.amplitude_encoder.GetPercent();
  current_state.mode = static_cast<tact::Modes>(peripherals.mode_encoder.GetPosition());
  current_state.slot = peripherals.slot_encoder.GetPosition();

  peripherals.display.DrawBootScreen();
  peripherals.buzzer.PlayInitSequence();
  delay(2000);

  peripherals.display.DrawMenuScreen(
    tact::Mode::GetName(static_cast<tact::Modes>(peripherals.mode_encoder.GetPosition())),
    peripherals.slot_encoder.GetPosition(),
    peripherals.amplitude_encoder.GetPercent()
  );
  delay(1000);
  
  previous_state = current_state;
  previous_state.mode = tact::Modes::undefined; //hence mode change will recognised

  tact::config::esp::DisableRadios();
}


void loop() {
  if (peripherals.mode_encoder.UpdateAvailable()) {
    current_state.mode = static_cast<tact::Modes>(peripherals.mode_encoder.GetPosition());
    auto mode_text = tact::Mode::GetName(current_state.mode);
    peripherals.display.DrawModeSelection(mode_text);
  }

  if (peripherals.slot_encoder.UpdateAvailable()) {
    // TODO: swop current tacton (#6): https://github.com/TactileVision/TactJam-firmware/issues/6
    current_state.slot = peripherals.slot_encoder.GetPosition();
    peripherals.display.DrawSlotSelection(current_state.slot);
    #ifdef TACT_DEBUG
    if (tact::config::kDebugLevel == tact::config::DebugLevel::verbose) {
      Serial.printf("slot: %u\n", current_state.slot);
    }
    #endif //TACT_DEBUG
  }

  if (peripherals.amplitude_encoder.UpdateAvailable()) {
    current_state.amplitude = peripherals.amplitude_encoder.Get12bit();
    current_state.amplitude_percent = peripherals.amplitude_encoder.GetPercent();
    peripherals.display.DrawAmplitude(current_state.amplitude_percent);
    #ifdef TACT_DEBUG
    if (tact::config::kDebugLevel == tact::config::DebugLevel::verbose) {
      Serial.printf("amplitude: %u(12bit) %u(percent)\n", current_state.amplitude, current_state.amplitude_percent);
    }
    #endif //TACT_DEBUG
  }

  if ((current_state.slot != previous_state.slot) || (current_state.mode != previous_state.mode)) {
    peripherals.actuator_driver.Update(0, 0);
    peripherals.button_leds.Update(0);
    peripherals.display.ClearContentTeaser();
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
  if (peripherals.buttons.UpdateAvailable()) {
    current_state.pressed_buttons = peripherals.buttons.Read();
    current_state.pressed_actuator_buttons = current_state.pressed_buttons >> 8u;
    current_state.pressed_menu_buttons = (current_state.pressed_buttons >> 5u) & 0x7u;
  
    #ifdef TACT_DEBUG
    if (tact::config::kDebugLevel >= tact::config::DebugLevel::verbose) {
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
    peripherals.button_leds.Update(current_state.pressed_actuator_buttons);
    peripherals.actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
    current_state.pressed_actuator_buttons = current_state.pressed_actuator_buttons;
  }

  if (previous_state.amplitude_percent != current_state.amplitude_percent) {
    peripherals.actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
  }

  // TODO: check if delay is needed
  delay(2);
}


void HandleRecPlayMode() {
  // TODO: amplitude modulation after recording (#10): https://github.com/TactileVision/TactJam-firmware/issues/10
  // TODO: delete current tacton (#12): https://github.com/TactileVision/TactJam-firmware/issues/12
  ReadButtons();

  if ((current_state.slot != previous_state.slot) || (current_state.mode != previous_state.mode)) {
    tacton_recorder_player.Reset();
  }

  if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
    if ((previous_state.pressed_menu_buttons & 4) == 4) {
      //menu button 1 pressed, start record
      tacton_recorder_player.RecordButtonPressed(current_state, peripherals.buzzer);
    }
    if ((previous_state.pressed_menu_buttons & 2) == 2) {
      //menu button 2 pressed, play
      tacton_recorder_player.PlayButtonPressed(peripherals.buzzer);
    }
    if ((previous_state.pressed_menu_buttons & 1) == 1) {
      //menu button 3 pressed, switch loop
      tacton_recorder_player.LoopButtonPressed(peripherals.buzzer);
    }
  }

  if (previous_state.pressed_actuator_buttons != current_state.pressed_actuator_buttons) {
    tacton_recorder_player.RecordSample(current_state, peripherals.buzzer);
  }
  if ((previous_state.amplitude != current_state.amplitude) && (current_state.pressed_actuator_buttons != 0)) {
    tacton_recorder_player.RecordSample(current_state, peripherals.buzzer);
  }
  tacton_recorder_player.PlaySample(current_state, peripherals.buzzer, peripherals.amplitude_encoder);


  //#ifdef TACT_DEBUG
  //Serial.println("Record and Play Mode is not implemented yet");
  //#endif //TACT_DEBUG

  // TODO: check if delay is needed
  delay(2);
}


void HandleDataTransferMode() {
  ReadButtons();

  if ((current_state.slot != previous_state.slot) || (current_state.mode != previous_state.mode)) {
    data_transfer.Reset();
  }

  if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
    if ((previous_state.pressed_menu_buttons & 4) == 4) {
      //menu button 1 pressed
      data_transfer.SendButtonPressed(current_state.slot);
    }
    if ((previous_state.pressed_menu_buttons & 2) == 2) {
      //menu button 2 pressed
      data_transfer.ReceiveButtonPressed(current_state.slot);
    }
  }

  data_transfer.Receive();

  delay(2);
}
