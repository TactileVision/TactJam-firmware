#include <Arduino.h>
#include <Adafruit_I2CDevice.h>

#include <config.h>
#include <debug.h>
#include <mode.h>
#include <state.h>
#include <peripherals.h>
#include <sampler.h>
#include <dataTransfer.h>

tact::Peripherals peripherals;
tact::State current_state;
tact::State previous_state;
tact::Sampler sampler(&peripherals, &current_state);
tact::DataTransfer data_transfer(&peripherals, &sampler);

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
  tact::debug::debug_stream << "TactJam Tact(" << GIT_TAG << "-" << GIT_REV << ")";
  tact::debug::println(tact::debug::debug_stream.str());
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
    #ifdef TACT_DEBUG
    tact::debug::debug_stream << "mode: " << mode_text.c_str();
    tact::debug::println(tact::debug::debug_stream.str());
    #endif //TACT_DEBUG
  }

  if (peripherals.slot_encoder.UpdateAvailable()) {
    current_state.slot = peripherals.slot_encoder.GetPosition();
    peripherals.display.DrawSlotSelection(current_state.slot);
    #ifdef TACT_DEBUG
    tact::debug::debug_stream << "slot: " << unsigned(current_state.slot);
    tact::debug::println(tact::debug::debug_stream.str());
    #endif //TACT_DEBUG
  }

  if (peripherals.amplitude_encoder.UpdateAvailable()) {
    current_state.amplitude = peripherals.amplitude_encoder.Get12bit();
    current_state.amplitude_percent = peripherals.amplitude_encoder.GetPercent();
    peripherals.display.DrawAmplitude(current_state.amplitude_percent);
    #ifdef TACT_DEBUG
    tact::debug::debug_stream << "amplitude: " << unsigned(current_state.amplitude) << "(12bit) " << unsigned(current_state.amplitude_percent) << "(percent)";
    tact::debug::println(tact::debug::debug_stream.str(), tact::debug::DebugLevel::verbose);
    #endif //TACT_DEBUG
  }

  if ((current_state.slot != previous_state.slot) || (current_state.mode != previous_state.mode)) {
    peripherals.actuator_driver.Update(0, 0);
    peripherals.button_leds.Update(0);
    if (current_state.mode == tact::Modes::jam) {
      const auto fill = String("/\\/\\/\\/\\/\\");
      peripherals.display.DrawContentTeaserDoubleLine(fill, fill);
    }
    else {
      peripherals.display.ClearContentTeaser();
    }
  }

  // needed to be located here to listen for message asking and anwering by message granting
  data_transfer.Receive(current_state);
  data_transfer.Send();

  switch (current_state.mode) {
    case tact::Modes::undefined :
      #ifdef TACT_DEBUG
      tact::debug::println("device is in an undefined mode");
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
    tact::debug::debug_stream << "active actuator buttons: " << unsigned(current_state.pressed_actuator_buttons) << "\tactive menu buttons: " << unsigned(current_state.pressed_actuator_buttons);
    tact::debug::println(tact::debug::debug_stream.str(), tact::debug::DebugLevel::verbose);
    #endif //TACT_DEBUG
  }
}


void HandleJamMode() {
  ReadButtons();
  if (previous_state.pressed_actuator_buttons != current_state.pressed_actuator_buttons) {
    peripherals.button_leds.Update(current_state.pressed_actuator_buttons);
    peripherals.actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
  }

  if (previous_state.amplitude_percent != current_state.amplitude_percent) {
    peripherals.actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);
  }

  if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
    if (previous_state.pressed_menu_buttons != 0) {
      peripherals.buzzer.PlayFail();
    }
  }

  // TODO: check if delay is needed
  delay(2);
}


void HandleRecPlayMode() {
  // TODO: amplitude modulation after recording (#10): https://github.com/TactileVision/TactJam-firmware/issues/10
  ReadButtons();

  if ((current_state.slot != previous_state.slot) || (current_state.mode != previous_state.mode)) {
    sampler.Reset();
  }

  if (previous_state.pressed_menu_buttons != current_state.pressed_menu_buttons) {
    if ((previous_state.pressed_menu_buttons & 4) == 4) {
      //menu button 1 pressed, start record
      sampler.RecordButtonPressed();
    }
    if ((previous_state.pressed_menu_buttons & 2) == 2) {
      //menu button 2 pressed, play
      sampler.PlayButtonPressed();
    }
    if ((previous_state.pressed_menu_buttons & 1) == 1) {
      //menu button 3 pressed, switch loop
      sampler.LoopButtonPressed();
    }
  }

  if (previous_state.pressed_actuator_buttons != current_state.pressed_actuator_buttons) {
    sampler.RecordSample();
  }
  if ((previous_state.amplitude != current_state.amplitude) && (current_state.pressed_actuator_buttons != 0)) {
    sampler.RecordSample();
  }
  sampler.PlaySample();

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

  //moved to void loop()
  //data_transfer.Receive();
  //data_transfer.Send();

  delay(2);
}
