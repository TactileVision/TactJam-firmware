#ifndef _TACTJAM_VTP_SAMPLER_
#define _TACTJAM_VTP_SAMPLER_

#include <vector>
#include <Arduino.h>
#include <vtp_codec.h>


namespace tactjam {
namespace data_format {

class VTPSampler {
private:
  uint64_t time_elapsed_;
  uint64_t time_last_write_;
  uint8_t sampling_rate_hz_;
  uint16_t tick_millis_;
  uint16_t amplitude_;
  uint8_t active_buttons_;
  std::vector<VTPInstructionWord> tacton;
  bool initialized_;

  void EncodeAndAddInstruction(const VTPInstructionV1* instruction) {
    VTPInstructionWord encoded_instruction;
    vtp_encode_instruction_v1(instruction, &encoded_instruction);
    tacton.push_back(encoded_instruction);
    time_last_write_ = time_elapsed_;
  }


public:
  VTPSampler() = delete;

  VTPSampler(uint8_t sampling_rate_hz) {
    sampling_rate_hz_ = sampling_rate_hz;
    initialized_ = false;
  }

  bool Initialize() {
    tick_millis_ = 1000/sampling_rate_hz_;
    time_elapsed_ = 0;
    initialized_ = true;
    return initialized_;
  }

  void Reset() {
    if (!initialized_) {
      return;
    }
    Initialize();
  }

  void WaitUntilNextTick() {
    if (!initialized_) {
      Initialize();
    }
    delay(tick_millis_);
    time_elapsed_ += tick_millis_;
  }

  void SetAmplitude(uint16_t amplitude) {
    if (!initialized_) {
      Initialize();
    }
    amplitude_ = amplitude;
  }

  void SetActiveButtons(uint8_t active_buttons) {
    if (!initialized_) {
      Initialize();
    }
    active_buttons_ = active_buttons;
  }

  bool UpdateAvailable(uint16_t active_buttons) {
    if (!initialized_) {
      Initialize();
    }
    return (active_buttons_ != active_buttons);
  }

  void WriteSample(uint8_t active_buttons) {
    if (!initialized_) {
      Initialize();
    }
    if (time_elapsed_ - time_last_write_ > 1023) {
      Serial.println("increment time");
      VTPInstructionV1 instruction;
      instruction.code = VTP_INST_INCREMENT_TIME;
      instruction.params.format_a.parameter_a = time_elapsed_ - time_last_write_;
      EncodeAndAddInstruction(&instruction);
    }
    for (uint8_t i = 0; i < 8; i++) {
      auto last_state = bitRead(active_buttons_, i);
      auto new_state = bitRead(active_buttons, i);
      if (last_state != new_state) {
        uint32_t new_amplitude = (new_state == 1) ? map(amplitude_, 0, 4095, 0, 1023) : 0;
        Serial.printf("%d:%d\t", i, new_amplitude);
        VTPInstructionV1 instruction;
        instruction.code = VTP_INST_SET_AMPLITUDE;
        instruction.params.format_b.channel_select = i;
        instruction.params.format_b.time_offset = time_elapsed_ - time_last_write_;
        instruction.params.format_b.parameter_a = new_amplitude;
        EncodeAndAddInstruction(&instruction);
      }
    }
    active_buttons_ = active_buttons;
  }

  uint64_t GetTactonSampleLength() {
    return tacton.size();
  }

  void PrintSample() {
    if (!initialized_) {
      Initialize();
    }
  }

};

}
}

#endif //_TACTJAM_VTP_SAMPLER_
