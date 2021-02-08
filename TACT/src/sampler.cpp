#include <sampler.h>
#include <sstream>

namespace tact {
  

Sampler::Sampler(Peripherals* peripherals) : tactons(TACTONS_COUNT_MAX) {
    peripherals_ = peripherals;
}


void Sampler::SetState(SamplerState state, bool force_display_update) {
  if ((state != this->state) || (force_display_update == true)) {
    std::string line_1;
    std::string line_2;
    switch(state) {
      case SamplerState::idle: 
        line_1.assign("idle");
        peripherals_->actuator_driver.Update(0, 0);
        peripherals_->button_leds.Update(0);
        break;
      case SamplerState::playing:
        line_1.assign("play");
        break;
      case SamplerState::recording:
        line_1.assign("rec");
        break;
    }
    if (loop_playback == true) {
      line_2.assign("loop on");
    }
    else {
      line_2.assign("loop off");
    }
    peripherals_->display.DrawContentTeaserDoubleLine(line_1.c_str(), line_2.c_str());
  }
  this->state = state;
}


void Sampler::Reset()  {
  SetState(SamplerState::idle, true);
  #ifdef TACT_DEBUG
  Serial.print("TactonRecorderPlayer::Reset\n");
  #endif //TACT_DEBUG
}


void Sampler::DeleteTacton(uint8_t slot) {
  if (slot > TACTONS_COUNT_MAX) {
    return;
  }
  tactons.at(slot).tacton_samples.clear();
}


void Sampler::RecordButtonPressed(State &current_state) {
  peripherals_->actuator_driver.Update(0, 0);
  peripherals_->button_leds.Update(0);
  if (state == SamplerState::recording) {
    SetState(SamplerState::idle);
    peripherals_->buzzer.PlayConfirm();
    peripherals_->buzzer.PlayConfirm();
    return;
  }
  tactons.at(current_state.slot).tacton_samples.clear();
  //tactons.at(current_state.slot).tacton_samples.reserve(TACTON_SAMPLES_MAX);
  time_start_milliseconds = 0;
  SetState(SamplerState::recording);

  peripherals_->buzzer.PlayConfirm();

  #ifdef TACT_DEBUG
  Serial.printf("TactonRecorderPlayer::RecordButtonPressed(): time_start_milliseconds=%ld\n", time_start_milliseconds);
  #endif //TACT_DEBUG
}


void Sampler::PlayButtonPressed() {
  peripherals_->actuator_driver.Update(0, 0);
  peripherals_->button_leds.Update(0);
  if (state == SamplerState::playing) {
    SetState(SamplerState::idle);
    peripherals_->buzzer.PlayConfirm();
    peripherals_->buzzer.PlayConfirm();
    return;
  }
  peripherals_->buzzer.PlayConfirm();
  time_start_milliseconds = millis();
  SetState(SamplerState::playing);
  index_play_next = 0;

  #ifdef TACT_DEBUG
  Serial.printf("TactonRecorderPlayer::PlayButtonPressed(): time_start_milliseconds=%ld\n", time_start_milliseconds);
  #endif //TACT_DEBUG
}


void Sampler::LoopButtonPressed() {
  //if (state != State::playing)
  //  return;
  loop_playback = !loop_playback;
  peripherals_->buzzer.PlayConfirm();
  SetState(state, true);
}


void Sampler::RecordSample(State &current_state) {

  if ( state != SamplerState::recording) {
    //actuator button is only allowed during recording
    if (current_state.pressed_actuator_buttons != 0) {
      peripherals_->buzzer.PlayFail();
    }
    return;
  }

  if (current_state.slot >= TACTONS_COUNT_MAX) {
    return;
  }
  //this avoids a time gap between start record and first actuator button press
  if (time_start_milliseconds == 0) {
    time_start_milliseconds = millis();
  }

  TactonSample tactonSample;
  tactonSample.time_milliseconds = millis() - time_start_milliseconds;
  tactonSample.buttons_state = current_state.pressed_actuator_buttons;
  tactonSample.amplitude_percent = current_state.amplitude_percent;

  tactons.at(current_state.slot).tacton_samples.push_back(tactonSample);

  peripherals_->button_leds.Update(current_state.pressed_actuator_buttons);
  peripherals_->actuator_driver.Update(current_state.pressed_actuator_buttons, current_state.amplitude);

  #ifdef TACT_DEBUG
  tactonSample.SerialPrint();
  #endif //TACT_DEBUG
}


void Sampler::PlaySample(State &current_state) {

  if (state != SamplerState::playing) {
    //peripherals_->buzzer.PlayFail();
    return;
  }

  if (current_state.slot >= TACTONS_COUNT_MAX) {
    return;
  }

  std::vector<TactonSample> *tacton_samples  = &tactons.at(current_state.slot).tacton_samples;

  if (index_play_next >= tacton_samples->size()) {
    //last sample has been played
    peripherals_->buzzer.PlayConfirm();
    peripherals_->buzzer.PlayConfirm();
    SetState(SamplerState::idle);
    if (loop_playback == true) {
      PlayButtonPressed();
    }
    return;
  }

  TactonSample *tacton_latest = NULL;
  unsigned long millis_current = millis() - time_start_milliseconds;
  for (int i = index_play_next; i < tacton_samples->size(); i++) {
    if (millis_current >= tacton_samples->at(i).time_milliseconds) {
      tacton_latest = &tacton_samples->at(i);
      index_play_next = i + 1;
    }
    else {
      break;
    }
  }

  if (tacton_latest != NULL) {
    peripherals_->actuator_driver.Update(tacton_latest->buttons_state, peripherals_->amplitude_encoder.PercentToLinearEncoder(tacton_latest->amplitude_percent));
    peripherals_->button_leds.Update(tacton_latest->buttons_state);
    #ifdef TACT_DEBUG
    Serial.printf("sample %d/%d: ", index_play_next, tacton_samples->size());
    tacton_latest->SerialPrint();
    #endif //TACT_DEBUG
  }
}


void Sampler::ToVTP(uint8_t slot, std::vector<unsigned char> &vector_out) {
  index_vtp_instruction = 0;

  std::vector<TactonSample> *tacton_samples  = &tactons.at(slot).tacton_samples;
  TactonSample tacton_sample_previous;
  VTPInstructionV1 instruction;
  VTPInstructionWord encodedInstruction;
  
  for (int i = 0; i < tacton_samples->size(); i++) {
    TactonSample *tacton_sample = &tacton_samples->at(i);

    #ifdef TACT_DEBUG
    if (config::kDebugLevel >= config::DebugLevel::verbose) {
      Serial.printf("ToVTP sample %d/%d: ", i + 1, tacton_samples->size());
      tacton_sample->SerialPrint();
    }
    #endif //TACT_DEBUG

    uint32_t time_diff = tacton_sample->time_milliseconds - tacton_sample_previous.time_milliseconds;
    if (time_diff > 0) {
      instruction.code = VTP_INST_INCREMENT_TIME;
      instruction.params.format_a.parameter_a = time_diff;

      #ifdef TACT_DEBUG
      if (config::kDebugLevel >= config::DebugLevel::verbose)
        Serial.printf("  VTP_INST_INCREMENT_TIME parameter_a=%ld\n", instruction.params.format_a.parameter_a);
      #endif //TACT_DEBUG

      if (vtp_encode_instruction_v1(&instruction, &encodedInstruction) != VTP_OK ) {
        #ifdef TACT_DEBUG
        Serial.printf("vtp_encode_instruction_v1 != VTP_OK\n");
        #endif //TACT_DEBUG
        return;
      }
      AddVTPInstruction(&encodedInstruction, vector_out);
    }
    
    for (uint8_t idx = 0; idx < 8; idx++) {
      if (((tacton_sample_previous.buttons_state >> idx)%2) != ((tacton_sample->buttons_state >> idx)%2)) {
        instruction.code = VTP_INST_SET_AMPLITUDE;
        instruction.params.format_b.time_offset = 0;
        instruction.params.format_b.channel_select = idx;
        instruction.params.format_b.parameter_a = ((tacton_sample->buttons_state >> idx)%2) == 1 ? (tacton_sample->amplitude_percent * 10) : 0;
      
        #ifdef TACT_DEBUG
        if (config::kDebugLevel >= config::DebugLevel::verbose)
          Serial.printf("  VTP_INST_SET_AMPLITUDE time_offset=%d  channel_select=%d  parameter_a=%d\n", instruction.params.format_b.time_offset, instruction.params.format_b.channel_select, instruction.params.format_b.parameter_a);
        #endif //TACT_DEBUG

        if (vtp_encode_instruction_v1(&instruction, &encodedInstruction) != VTP_OK) {
          #ifdef TACT_DEBUG
          Serial.printf("vtp_encode_instruction_v1 != VTP_OK\n");
          #endif //TACT_DEBUG
          return;
        }
        AddVTPInstruction(&encodedInstruction, vector_out);
      }
    }
 
    tacton_sample_previous = *tacton_sample;
  }
}


void Sampler::AddVTPInstruction(VTPInstructionWord* encoded_instruction_word, std::vector<unsigned char> &vector_out) {
  unsigned char buffer[4];
  vtp_write_instruction_words(1, encoded_instruction_word, buffer);

  vector_out.push_back(buffer[0]);
  vector_out.push_back(buffer[1]);
  vector_out.push_back(buffer[2]);
  vector_out.push_back(buffer[3]);

  //{ //decoding test to slot 2
  //  VTPInstructionWord out;
  //  vtp_read_instruction_words(1, buffer, &out);
  //  FromVTP(2, &out, index_vtp_instruction);
  //}

  index_vtp_instruction++; //value is resetted in ToVTP(..)
}


int Sampler::FromVTP(uint8_t slot, VTPInstructionWord* encoded_instruction_word, uint32_t index_of_instruction) {
  std::vector<TactonSample> *tacton_samples  = &tactons.at(slot).tacton_samples;
  if (index_of_instruction == 0) {
    time_vtp_instruction_milliseconds = 0;
    tacton_samples->clear();
  }

  VTPInstructionV1 instruction;
  if (vtp_decode_instruction_v1(*encoded_instruction_word, &instruction) != VTP_OK) {
    tacton_samples->clear();
    #ifdef TACT_DEBUG
    Serial.printf("ERROR: vtp_decode_instruction_v1 != VTP_OK\n");
    #endif //TACT_DEBUG
    return -1;
  }

  TactonSample* tacton_sample;
  if (tacton_samples->empty() == true) {
    tacton_samples->push_back(TactonSample());
  }

  tacton_sample = &tacton_samples->at(tacton_samples->size()-1);

  if (instruction.code == VTP_INST_INCREMENT_TIME) {
    time_vtp_instruction_milliseconds+=instruction.params.format_a.parameter_a;
    TactonSample tacton_sample_new = *tacton_sample;
    tacton_sample_new.time_milliseconds = time_vtp_instruction_milliseconds;
    tacton_samples->push_back(tacton_sample_new);
  }
  else if (instruction.code == VTP_INST_SET_AMPLITUDE) {
    uint8_t button = instruction.params.format_b.channel_select;
    uint16_t amplitude = instruction.params.format_b.parameter_a;
    if (amplitude == 0) {
      tacton_sample->buttons_state &= ~(1 << button);
    }
    else {
      tacton_sample->buttons_state |= (1 << button);
      tacton_sample->amplitude_percent = amplitude / 10;
    }
  }
  else {
    tacton_samples->clear();
    #ifdef TACT_DEBUG
    Serial.printf("ERROR: instruction.code not implemented\n");
    #endif //TACT_DEBUG
    return -2;
  }
  
  #ifdef TACT_DEBUG
  if (config::kDebugLevel >= config::DebugLevel::verbose) {
    Serial.printf(" FromVTP sample %d: ", tacton_samples->size());
    tacton_samples->at(tacton_samples->size()-1).SerialPrint();
  }
  #endif //TACT_DEBUG

  return 0;
}


std::string Sampler::GetTactonListAsString(void) {
  std::ostringstream ss_out;
  for (int i = 0; i < tactons.size(); i++) {
    if (tactons.at(i).tacton_samples.size() > 0) {
      if (ss_out.str().empty() == false) {
        ss_out << ",";
      }
      ss_out << i;
    }
  }
  return ss_out.str();
}

} // namespace tact