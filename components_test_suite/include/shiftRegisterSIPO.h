#ifndef _TACTJAM_SHIFTREGISTER_SIPO_
#define _TACTJAM_SHIFTREGISTER_SIPO_

/*
 * A shift register is used to reduce the number of pins
 * for driving 8 LEDs. The values of all LEDs can be stored
 * in a single 8-bit data type (e.g. uint8_t).
 * Order of LEDs:
 *  | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 |
 * 
 */

#include <Arduino.h>

namespace tactjam {
namespace shiftregister {
namespace sipo {
  
class SN74HC595 {
  private:
    uint8_t latch_pin_;
    uint8_t clock_pin_;
    uint8_t data_pin_;
    uint8_t bit_order_;
    uint32_t delay_millis_;
    bool initialized_;

  public:
    SN74HC595() {
      // default configuration SN74HC595(18, 19, 5)
      *this = SN74HC595(27, 26, 14);
    }

    SN74HC595(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin, uint32_t delay_millis = 50, uint8_t bit_order = LSBFIRST) {
      latch_pin_ = latch_pin;
      clock_pin_ = clock_pin;
      data_pin_ = data_pin;
      bit_order_ = bit_order;
      delay_millis_ = delay_millis;
      initialized_ = false;
    }

    ~SN74HC595() = default;

    void ChangeConfig(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin, uint32_t delay_millis, uint8_t bit_order) {
      latch_pin_ = latch_pin;
      clock_pin_ = clock_pin;
      data_pin_ = data_pin;
      bit_order_ = bit_order;
      delay_millis_ = delay_millis;
      Initialize();
    }

    void ChangePins(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin) {
      latch_pin_ = latch_pin;
      clock_pin_ = clock_pin;
      data_pin_ = data_pin;
      Initialize();
    }

    void Initialize() {
      pinMode(latch_pin_, OUTPUT);
      pinMode(clock_pin_, OUTPUT);
      pinMode(data_pin_, OUTPUT);
      initialized_ = true;
    }

    void Update(uint8_t value) {
      if (!initialized_) {
        Initialize();
      }
      digitalWrite(latch_pin_, LOW);
      shiftOut(data_pin_, clock_pin_, bit_order_, value);
      digitalWrite(latch_pin_, HIGH);
    }

    void Test() {
      if (!initialized_) {
        Initialize();
      }
      for (uint8_t i=0; i<255; i++) {
        Update(i);
        delay(delay_millis_);
      }
      Update(0);
    }
};

}
}
}

#endif //_TACTJAM_SHIFTREGISTER_SIPO_
