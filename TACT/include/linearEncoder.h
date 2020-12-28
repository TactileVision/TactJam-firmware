#ifndef _TACT_LINEAR_ENCODER_
#define _TACT_LINEAR_ENCODER_

#include <Arduino.h>

namespace tact {

class LinearEncoder {
  private:
    uint8_t pin_;
    uint16_t value_;
    uint16_t filter_width_;
    bool initialized_;

  public:
    LinearEncoder() = delete;
    LinearEncoder(uint8_t pin, uint16_t filter_width = 30);
    ~LinearEncoder() = default;

    void Initialize();
    bool UpdateAvailable();
    uint16_t Read();
    uint8_t GetPercent(bool update = true);
    uint8_t Get8bit(bool update = true);
    uint16_t Get12bit(bool update = true);
};

}

#endif //_TACT_LINEAR_ENCODER_
