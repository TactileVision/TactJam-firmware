#include <tactons.h>

namespace tact {


void TactonSample::SerialPrint() {
  Serial.printf("TactonSample: time_milliseconds=%d  buttons_state=", time_milliseconds);
  Serial.print(buttons_state, BIN);
  Serial.printf("  amplitude=%d\n", amplitude_percent);
}
  

TactonRecorderPlayer::TactonRecorderPlayer(tact::Display* display, PCA9685* actuator_driver, SN74HC595* button_leds) :
  tactons(TACTONS_COUNT_MAX), display(display), actuator_driver(actuator_driver), button_leds(button_leds) {
}


void TactonRecorderPlayer::SetState(State state, bool force_display_update) {
  if (state != this->state || force_display_update == true) {
    std::string line_1;
    std::string line_2;
    switch(state) {
      case State::idle: 
        line_1.assign("idle");
        actuator_driver->Update(0, 0);
        button_leds->Update(0);
        break;
      case State::playing:
        line_1.assign("play");
        break;
      case State::recording:
        line_1.assign("rec");
        break;
    }
    if ( loop_playback == true )
      line_2.assign("loop on");
    else
      line_2.assign("loop off");
    display->DrawContentTeaserDoubleLine(line_1.c_str(), line_2.c_str());
  }
  this->state = state;
}


void TactonRecorderPlayer::Reset()  {
  SetState(State::idle, true);
  #ifdef TACT_DEBUG
  Serial.println("actonRecorderPlayer::Reset");
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::RecordButtonPressed(tact::State &current_state, tact::Buzzer &buzzer) {
  actuator_driver->Update(0, 0);
  button_leds->Update(0);
  if (state == State::recording) {
    SetState(State::idle);
    buzzer.PlayConfirm();
    buzzer.PlayConfirm();
    return;
  }
  tactons.at(current_state.slot).tacton_samples.clear();
  //tactons.at(current_state.slot).tacton_samples.reserve(TACTON_SAMPLES_MAX);
  time_start_milliseconds = millis();
  SetState(State::recording);

  buzzer.PlayConfirm();

  #ifdef TACT_DEBUG
  Serial.printf("RecordButtonPressed(): time_start_milliseconds=%ld\n", time_start_milliseconds);
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::PlayButtonPressed(tact::Buzzer &buzzer) {
  actuator_driver->Update(0, 0);
  button_leds->Update(0);
  buzzer.PlayConfirm();
  time_start_milliseconds = millis();
  SetState(State::playing);
  index_play_next = 0;

  #ifdef TACT_DEBUG
  Serial.printf("PlayButtonPressed(): time_start_milliseconds=%ld\n", time_start_milliseconds);
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::LoopButtonPressed(tact::Buzzer &buzzer) {
  //if (state != State::playing)
  //  return;
  loop_playback = !loop_playback;
  buzzer.PlayConfirm();
  SetState(state, true);
}


void TactonRecorderPlayer::RecordSample(tact::State &current_state, tact::Buzzer &buzzer) {

  if ( state == State::playing) {
    //actuator button is pressed during playback, this is not allowed
    //buzzer.PlayFail();
    return;
  }

  if ( state == State::idle) {
    //directly start recording, if actuator button is pressed in idle mode
    RecordButtonPressed(current_state, buzzer);
  }

  if ( current_state.slot >= TACTONS_COUNT_MAX)
    return;

  TactonSample tactonSample;
  tactonSample.time_milliseconds = millis() - time_start_milliseconds;
  tactonSample.buttons_state = current_state.pressed_actuator_buttons;
  tactonSample.amplitude_percent = current_state.amplitude_percent;

  tactons.at(current_state.slot).tacton_samples.push_back(tactonSample);

  button_leds->Update(current_state.pressed_actuator_buttons);
  actuator_driver->Update(current_state.pressed_actuator_buttons, current_state.amplitude);

  #ifdef TACT_DEBUG
  tactonSample.SerialPrint();
  #endif //TACT_DEBUG
}


void TactonRecorderPlayer::PlaySample(tact::State &current_state, tact::Buzzer &buzzer, tact::LinearEncoder &amplitude_encoder) {

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
    SetState(State::idle);
    if (loop_playback == true)
      PlayButtonPressed(buzzer);
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
    actuator_driver->Update(tacton_latest->buttons_state, amplitude_encoder.PercentToLinearEncoder(tacton_latest->amplitude_percent));
    button_leds->Update(tacton_latest->buttons_state);
    #ifdef TACT_DEBUG
    Serial.printf("sample %d/%d: ", index_play_next, tacton_samples->size());
    tacton_latest->SerialPrint();
    #endif //TACT_DEBUG
  }
}


void TactonRecorderPlayer::ToVTP(uint8_t slot) {
  index_vtp_instruction = 0;

  std::vector<TactonSample> *tacton_samples  = &tactons.at(slot).tacton_samples;
  TactonSample tacton_sample_previous;
  VTPInstructionV1 instruction;
  VTPInstructionWord encodedInstruction;
  
  for (int i = 0; i < tacton_samples->size(); i++) {
    TactonSample *tacton_sample = &tacton_samples->at(i);

    #ifdef TACT_DEBUG
    Serial.printf("ToVTP sample %d/%d: ", i + 1, tacton_samples->size());
    tacton_sample->SerialPrint();
    #endif //TACT_DEBUG

    uint32_t time_diff = tacton_sample->time_milliseconds - tacton_sample_previous.time_milliseconds;
    if (time_diff > 0) {
      instruction.code = VTP_INST_INCREMENT_TIME;
      instruction.params.format_a.parameter_a = time_diff;

      #ifdef TACT_DEBUG
      Serial.printf("  VTP_INST_INCREMENT_TIME parameter_a=%ld\n", instruction.params.format_a.parameter_a);
      #endif //TACT_DEBUG

      if (vtp_encode_instruction_v1(&instruction, &encodedInstruction) != VTP_OK ) {
        Serial.printf("vtp_encode_instruction_v1 != VTP_OK\n");
        return;
      }
      TransferVTPInstructionToPC(&encodedInstruction);
    }
    
    for (uint8_t idx = 0; idx < 8; idx++) {
      if (((tacton_sample_previous.buttons_state >> idx)%2) != ((tacton_sample->buttons_state >> idx)%2)) {
        instruction.code = VTP_INST_SET_AMPLITUDE;
        instruction.params.format_b.time_offset = 0;
        instruction.params.format_b.channel_select = idx;
        instruction.params.format_b.parameter_a = ((tacton_sample->buttons_state >> idx)%2) == 1 ? tacton_sample->amplitude_percent * 10:0;
      
        #ifdef TACT_DEBUG
        Serial.printf("  VTP_INST_SET_AMPLITUDE time_offset=%d  channel_select=%d  parameter_a=%d\n", instruction.params.format_b.time_offset, instruction.params.format_b.channel_select, instruction.params.format_b.parameter_a);
        #endif //TACT_DEBUG

        if ( vtp_encode_instruction_v1(&instruction, &encodedInstruction) != VTP_OK) {
          Serial.printf("vtp_encode_instruction_v1 != VTP_OK\n");
          return;
        }
        TransferVTPInstructionToPC(&encodedInstruction);
      }
    }
 
    tacton_sample_previous = *tacton_sample;
  }
}


void TactonRecorderPlayer::TransferVTPInstructionToPC(VTPInstructionWord* encodedInstructionWord) {
  unsigned char buffer[4];
  vtp_write_instruction_words(1, encodedInstructionWord, buffer);
  //TODO: write instruction word to PC connection
  //Serial.write(buffer, 4);

  { //decoding test to slot 2
    VTPInstructionWord out;
    vtp_read_instruction_words(1, buffer, &out);
    FromVTP(2, &out, index_vtp_instruction);
  }

  index_vtp_instruction++; //value is resetted in ToVTP(..)
}


void TactonRecorderPlayer::FromVTP(uint8_t slot, VTPInstructionWord* encodedInstructionWord, uint32_t index_of_instruction) {
  std::vector<TactonSample> *tacton_samples  = &tactons.at(slot).tacton_samples;
  if ( index_of_instruction == 0 ) {
    time_vtp_instruction_milliseconds = 0;
    tacton_samples->clear();
  }

  VTPInstructionV1 instruction;
  if ( vtp_decode_instruction_v1(*encodedInstructionWord, &instruction) != VTP_OK) {
    Serial.printf("ERROR: vtp_decode_instruction_v1 != VTP_OK\n");
    return;
  }

  TactonSample* tacton_sample;
  if (tacton_samples->empty() == true)
    tacton_samples->push_back(TactonSample());
 
  tacton_sample = &tacton_samples->at(tacton_samples->size()-1);

  if ( instruction.code == VTP_INST_INCREMENT_TIME) {
    time_vtp_instruction_milliseconds+=instruction.params.format_a.parameter_a;
    TactonSample tacton_sample_new = *tacton_sample;
    tacton_sample_new.time_milliseconds = time_vtp_instruction_milliseconds;
    tacton_samples->push_back(tacton_sample_new);
  } else if ( instruction.code == VTP_INST_SET_AMPLITUDE) {
    uint8_t button = instruction.params.format_b.channel_select;
    uint16_t amplitude = instruction.params.format_b.parameter_a;
    if ( amplitude == 0)
      tacton_sample->buttons_state &= ~(1 << button);
    else {
      tacton_sample->buttons_state |= (1 << button);
      tacton_sample->amplitude_percent = amplitude / 10;
    }
  } else Serial.printf("ERROR: instruction.code not implemented\n");
  
  #ifdef TACT_DEBUG
  Serial.printf(" FromVTP sample %d: ", tacton_samples->size());
  tacton_samples->at(tacton_samples->size()-1).SerialPrint();
  #endif //TACT_DEBUG
}

} // namespace tact