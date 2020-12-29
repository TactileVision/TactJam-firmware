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

    /**
     * @brief 
     * 
     * @return uint8_t 
     */
    uint8_t Read();

  public:
    RotarySwitch3Pos() = delete;
    RotarySwitch3Pos(uint8_t pin);
    ~RotarySwitch3Pos() = default;

    /**
     * @brief Initialize the switch.
     * 
     */
    void Initialize();

    /**
     * @brief Get the selected position of the switch.
     * 
     * @return uint8_t the postion of the switch.
     */
    uint8_t GetPosition();

    /**
     * @brief Check if the position of the switch has changed.
     * 
     * @return true the position has changed.
     * @return false the position is still the same as before.
     */
    bool UpdateAvailable();
};

}

#endif //_TACT_ROTARY_SWITCH_
