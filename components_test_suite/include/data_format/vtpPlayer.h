#ifndef _TACTJAM_VTP_PLAYER_
#define _TACTJAM_VTP_PLAYER_

#include <vector>
#include <Arduino.h>
#include <vtp_fold.h>


namespace tactjam {
namespace data_format {

#define N_CHANNELS (4)
#define N_INSTRUCTIONS (8)

class VTPPlayer {
private:
  const VTPInstructionWord example_instructions_[N_INSTRUCTIONS] = {
    0x100000ea, 0x2000007b, 0x10200159, 0x1020c9c8,
    0x10100315, 0x000007d0, 0x200000ea, 0x10200237
  };
  unsigned int amplitudes[N_CHANNELS], frequencies[N_CHANNELS];
  VTPInstructionV1 decoded_instructions_[N_INSTRUCTIONS];
  VTPAccumulatorV1 accumulator_;
  const VTPInstructionV1* current_instruction_;
  unsigned long time_elapsed_;
  uint8_t sampling_rate_hz_;
  uint16_t tick_millis_;
  bool initialized_;


public:
  VTPPlayer() = delete;

  VTPPlayer(uint8_t sampling_rate_hz) {
    sampling_rate_hz_ = sampling_rate_hz;
    initialized_ = false;
  }

  bool Initialize() {
    accumulator_.n_channels = N_CHANNELS;
    accumulator_.amplitudes = amplitudes;
    accumulator_.frequencies = frequencies;
    accumulator_.milliseconds_elapsed = 0;
    memset(amplitudes, 0, N_CHANNELS);
    memset(frequencies, 0, N_CHANNELS);
    current_instruction_ = decoded_instructions_;
    tick_millis_ = 1000/sampling_rate_hz_;
    time_elapsed_ = 0;
    if (vtp_decode_instructions_v1(example_instructions_, decoded_instructions_, N_INSTRUCTIONS) != VTP_OK) {
      Serial.println("Invalid instructions - could not decode!");
      initialized_ = false;
    }
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
      return;
    }
    delay(tick_millis_);
    time_elapsed_ += tick_millis_;
  }

  void GetNextSample() {
    if (!initialized_) {
      return;
    }
    VTPError error;
    size_t n_processed;
    if ((error = vtp_fold_until_v1(&accumulator_, current_instruction_, decoded_instructions_ + N_INSTRUCTIONS - current_instruction_, time_elapsed_, &n_processed)) != VTP_OK) {
      Serial.print("Error while folding - Code ");
      Serial.print(error);
      Serial.print("\n");
    }  
    current_instruction_ += n_processed;
  }

  void PrintSample() {
    if (!initialized_) {
      return;
    }
    int i = 0;
    Serial.printf("time elapsed: %ums\t", time_elapsed_);
    Serial.print("amplitudes: ");
    for (i=0; i < accumulator_.n_channels; i++) {
      Serial.printf("%u ", accumulator_.amplitudes[i]);
    }
    Serial.print("\tfrequencies: ");
    for (i=0; i < accumulator_.n_channels; i++) {
      Serial.printf("%u ", accumulator_.frequencies[i]);
    }
    Serial.println();
  }

};

}
}

#endif //_TACTJAM_VTP_PLAYER_
