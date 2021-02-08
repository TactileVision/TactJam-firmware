#ifndef _TACT_TACTON_
#define _TACT_TACTON_

#include <vector>
#include <Arduino.h>

namespace tact {

struct TactonSample {
  uint32_t time_milliseconds = 0;
  uint8_t buttons_state = 0;
  uint8_t amplitude_percent = 0;

  void SerialPrint() {
    Serial.printf("TactonSample: time_milliseconds=%d  buttons_state=", time_milliseconds);
    Serial.print(buttons_state, BIN);
    Serial.printf("  amplitude=%d\n", amplitude_percent);
  }
};


struct Tacton {
  uint8_t slot;
  std::vector<TactonSample> tacton_samples;
};

} // namespace tact

#endif //_TACT_TACTON_
