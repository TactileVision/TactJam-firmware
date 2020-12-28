#ifndef _TACT_ROTARY_SWITCH_
#define _TACT_ROTARY_SWITCH_

#include <Arduino.h>

namespace tact {

class RotarySwitch3Pos {
  private:
    const uint16_t kBoundPos0 = 0;
    const uint16_t kBoundPos1 = 250;
    const uint16_t kBoundPos2 = 750;
    uint8_t pin_;
    uint8_t position_;
    bool initialized_;

    uint8_t Read();

  public:
    RotarySwitch3Pos() = delete;
    RotarySwitch3Pos(uint8_t pin);
    ~RotarySwitch3Pos() = default;

    void Initialize();
    uint8_t GetPosition();
    bool UpdateAvailable();
};

}

#endif //_TACT_ROTARY_SWITCH_
