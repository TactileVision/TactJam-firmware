#ifndef _TACT_M74HC166_
#define _TACT_M74HC166_

#include <Arduino.h>

namespace tact {

class M74HC166 {
  private:
    uint8_t latch_pin_;
    uint8_t clock_pin_;
    uint8_t data_pin_;
    uint16_t data_;
    bool initialized_;

  public:
    M74HC166() = delete;
    M74HC166(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin);
    ~M74HC166() = default;

    void Initialize();
    uint16_t Read();
};

}

#endif //_TACT_M74HC166_
