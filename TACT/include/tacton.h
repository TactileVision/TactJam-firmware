#ifndef _TACT_TACTON_
#define _TACT_TACTON_

#include <vector>
#include <Arduino.h>

#include "debug.h"


namespace tact {

struct TactonSample {
  uint32_t time_milliseconds = 0;
  uint8_t buttons_state = 0;
  uint8_t amplitude_percent = 0;

  void SerialPrint() {
    #ifdef TACT_DEBUG
    debug::debug_stream << "TactonSample: time_milliseconds=" << time_milliseconds << "  buttons_state=" << unsigned(buttons_state) << "  amplitude=" << unsigned(amplitude_percent);
    debug::println(debug::debug_stream.str(), debug::DebugLevel::verbose);
    #endif //TACT_DEBUG
  }
};


struct Tacton {
  //uint8_t slot;
  std::vector<TactonSample> tacton_samples;
};

} // namespace tact

#endif //_TACT_TACTON_
