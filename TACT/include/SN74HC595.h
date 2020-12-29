#ifndef _TACT_SN74HC595_
#define _TACT_SN74HC595_

#include <Arduino.h>

namespace tact {

class SN74HC595 {
  private:
    uint8_t latch_pin_;
    uint8_t clock_pin_;
    uint8_t data_pin_;
    uint8_t bit_order_;
    bool initialized_;

  public:
    SN74HC595() = delete;
    SN74HC595(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin, uint8_t bit_order = LSBFIRST);
    ~SN74HC595() = default;

    /**
     * @brief Initialize the shift register.
     * 
     */
    void Initialize();

    /**
     * @brief Send a bit pattern to the shift register.
     * 
     * @param active_positions the bit pattern (1=activated).
     */
    void Update(uint8_t active_positions);
};

}

#endif //_TACT_SN74HC595_
