#include "tactons.h"

namespace tact {


void TactonSample::SerialPrint() {
  Serial.printf("TactonSample: time_milliseconds=%d  buttons_state=", time_milliseconds);
  Serial.print(buttons_state, BIN);
  Serial.printf("  amplitude=%d\n", amplitude_percent);
}


TactonRecorderPlayer::TactonRecorderPlayer() : tactons(TACTONS_COUNT_MAX) {
}


void TactonRecorderPlayer::Reset(void)  {
  state = State::idle;
  #ifdef TACT_DEBUG
  Serial.println("actonRecorderPlayer::Reset");
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::RecordButtonPressed(tact::State &current_state, tact::Buzzer &buzzer) {
  if (state == State::recording) {
    state = State::idle;
    buzzer.PlayConfirm();
    buzzer.PlayConfirm();
    return;
  }
  tactons.at(current_state.slot).tacton_samples.clear();
  //tactons.at(current_state.slot).tacton_samples.reserve(TACTON_SAMPLES_MAX);
  time_start_milliseconds = millis();
  state = State::recording;

  buzzer.PlayConfirm();

  #ifdef TACT_DEBUG
  Serial.printf("RecordButtonPressed(): time_start_milliseconds=%ld\n", time_start_milliseconds);
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::PlayButtonPressed(tact::Buzzer &buzzer) {
  buzzer.PlayConfirm();
  time_start_milliseconds = millis();
  state = State::playing;
  index_play_next = 0;

  #ifdef TACT_DEBUG
  Serial.printf("PlayButtonPressed(): time_start_milliseconds=%ld\n", time_start_milliseconds);
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::RecordSample(tact::State &current_state, tact::Buzzer &buzzer, tact::PCA9685 &actuator_driver, tact::SN74HC595 &button_leds) {

  if ( state != State::recording) {
    //actuator button is pressed during recording, this is not allowed
    buzzer.PlayFail();
    return;
  }

  if ( current_state.slot >= TACTONS_COUNT_MAX)
    return;

  TactonSample tactonSample;
  tactonSample.time_milliseconds = millis() - time_start_milliseconds;
  tactonSample.buttons_state = current_state.pressed_actuator_buttons;
  tactonSample.amplitude_percent = current_state.amplitude_percent;

  tactons.at(current_state.slot).tacton_samples.push_back(tactonSample);

  button_leds.Update(current_state.pressed_actuator_buttons);
  actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);

  #ifdef TACT_DEBUG
  tactonSample.SerialPrint();
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::PlaySample(tact::State &current_state, tact::Buzzer &buzzer, tact::PCA9685 &actuator_driver, 
                                      tact::SN74HC595 &button_leds, tact::LinearEncoder &amplitude_encoder) {

  if ( state != State::playing) {
    //buzzer.PlayFail();
    return;
  }

  if ( current_state.slot >= TACTONS_COUNT_MAX)
    return;

  std::vector<TactonSample> *tacton_samples  = &tactons.at(current_state.slot).tacton_samples;

  if ( index_play_next >= tacton_samples->size()) {
    //last sample has been played
    buzzer.PlayConfirm();
    buzzer.PlayConfirm();
    state = State::idle;
    return;
  }

  TactonSample *tacton_latest = NULL;
  unsigned long millis_current = millis() - time_start_milliseconds;
  for (int i = index_play_next; i < tacton_samples->size(); i++) {
    if (millis_current >= tacton_samples->at(i).time_milliseconds) {
      tacton_latest = &tacton_samples->at(i);
      index_play_next = i + 1;
    } else {
      break;
    }
  }

  if ( tacton_latest != NULL ) {
    actuator_driver.Update(tacton_latest->buttons_state, amplitude_encoder.PercentToLinearEncoder(tacton_latest->amplitude_percent));
    button_leds.Update(tacton_latest->buttons_state);
    #ifdef TACT_DEBUG
    Serial.printf("sample %d/%d: ", index_play_next, tacton_samples->size());
    tacton_latest->SerialPrint();
    #endif //TACT_DEBUG
  }
}

}