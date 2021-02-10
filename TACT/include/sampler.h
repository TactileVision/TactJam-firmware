#ifndef _TACT_SAMPLER_
#define _TACT_SAMPLER_

#include <Arduino.h>
#include <vector>
#include "peripherals.h"
#include "tacton.h"
#include "state.h"

extern "C" {
#include <vtp/codec.h>
}

namespace tact {

//#define TACTON_SAMPLES_MAX 10000
#define TACTONS_COUNT_MAX 4

class Sampler {
  public:
    enum class SamplerState {
      idle,
      recording,
      playing
    };

    Sampler(Peripherals* peripherals);

    void SetState(SamplerState state, bool force_display_update = false);
    void Reset();
    void DeleteTacton(uint8_t slot);
    void RecordButtonPressed(State &current_state);
    void PlayButtonPressed();
    void LoopButtonPressed();
    void RecordSample(State &current_state);
    void PlaySample(State &current_state);

    void ToVTP(uint8_t slot, std::vector<uint8_t> &vector_out);
    void AddVTPInstruction(VTPInstructionWord* encoded_instruction_word, std::vector<uint8_t> &vector_out);
    /**
     *  @param index_of_instruction used for initialisation if index is 0 and may be used for debugging
     */
    int FromVTP(uint8_t slot, VTPInstructionWord* encoded_instruction_word, uint32_t index_of_instruction);
    std::string GetTactonListAsString(void);

  private:
    std::vector<Tacton> tactons;
    Peripherals* peripherals_;

    unsigned long time_start_milliseconds = 0;
    uint32_t index_play_next = 0;
    SamplerState state = SamplerState::idle; // use method SetState to change the state
    bool loop_playback = false;

    //VTP based variables
    unsigned long time_vtp_instruction_milliseconds = 0;
    uint32_t index_vtp_instruction = 0;
};


} //namespace tact


#endif //_TACT_SAMPLER_