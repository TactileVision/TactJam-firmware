#ifndef _TACT_TACTONS_
#define _TACT_TACTONS_

#include <Arduino.h>
#include <vector>
#include "buzzer.h"
#include "PCA9685.h"
#include "SN74HC595.h"
#include "linearEncoder.h"
#include "display.h"
#include "state.h"

namespace tact {

//#define TACTON_SAMPLES_MAX 10000
#define TACTONS_COUNT_MAX 8

class TactonSample {
  public:
    uint32_t time_milliseconds = 0;
    uint8_t buttons_state = 0;
    uint8_t amplitude_percent = 0;

    void SerialPrint(void);
};

class Tacton {
  public:
    std::vector<TactonSample> tacton_samples;
};

class TactonRecorderPlayer {
  public:
    enum class State {
      idle,
      recording,
      playing
    };

    TactonRecorderPlayer(tact::Display* display, PCA9685* actuator_driver, SN74HC595* button_leds);

    void SetState(State state, bool force_display_update = false);
    void Reset();
    void RecordButtonPressed(tact::State &current_state, tact::Buzzer &buzzer);
    void PlayButtonPressed(tact::Buzzer &buzzer);
    void LoopButtonPressed(tact::Buzzer &buzzer);
    void RecordSample(tact::State &current_state, tact::Buzzer &buzzer);
    void PlaySample(tact::State &current_state, tact::Buzzer &buzzer, tact::LinearEncoder &amplitude_encoder);


  private:
    std::vector<Tacton> tactons;
    Display* display;
    PCA9685* actuator_driver;
    SN74HC595* button_leds;

    unsigned long time_start_milliseconds = 0;
    uint32_t index_play_next = 0;
    State state = State::idle; // use method SetState to change the state
    bool loop_playback = false;
};


} //namespace tact


#endif //_TACT_TACTONS_