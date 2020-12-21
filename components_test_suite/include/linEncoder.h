#ifndef _TACTJAM_LIN_ENCODER_
#define _TACTJAM_LIN_ENCODER_

#include <Arduino.h>

namespace tactjam {
namespace encoder {

class LinEncoder {
  private:
    uint8_t pin_;
    uint16_t value_;
    uint16_t filter_width_;
    bool initialized_;

  public:
    LinEncoder() = delete;

    LinEncoder(uint8_t pin, uint16_t filter_width = 20) {
      pin_ = pin;
      filter_width_ = filter_width;
      initialized_ = false;
    }

    void Initialize() {
      pinMode(pin_, INPUT);
      value_ = 0;
      initialized_ = true;
      Read();
    }

    bool UpdateAvailable() {
      auto val = analogRead(pin_);
      auto lower = (value_-filter_width_ > 0) ? value_-filter_width_ : 0;
      auto upper = (value_+filter_width_ < 4096) ? value_+filter_width_ : 4095;
      return ((val < lower) || (val > upper));
    }

    uint16_t Read() {
      if (!initialized_) {
        Initialize();
      }
      value_ = analogRead(pin_);
      return value_;
    }

    uint8_t GetPercent(bool update = true) {
      if (update) {
        Read();
      }
      return (uint8_t)map(value_, 0, 4095, 0, 100);
    }

    uint8_t Get8bit(bool update = true) {
      if (update) {
        Read();
      }
      return (uint8_t)map(value_, 0, 4095, 0, 255);
    }

    uint16_t Get12bit(bool update = true) {
      if (update) {
        Read();
      }
      return value_;
    }
};


class LinEncoderSwitch {
  private:
    uint8_t pin_;
    uint8_t num_states_;
    uint8_t state_;
    bool initialized_;

  public:
    LinEncoderSwitch() = delete;

    LinEncoderSwitch(uint8_t pin, uint8_t num_states) {
      pin_ = pin;
      num_states_ = (num_states > 0) ? num_states : 1;
      initialized_ = false;
    }

    void Initialize() {
      pinMode(pin_, INPUT);
      state_ = 0;
      initialized_ = true;
      GetState();
    }

    uint8_t GetState() {
      if (!initialized_) {
        Initialize();
      }
      //state_ = map(analogRead(pin_), 0, 4095, 0, num_states_-1);
      
      int val = analogRead(pin_);

      if (val >= 0 && val < 250) {
        state_ = 0;
      }
      else if (val >= 250 && val < 750) {
        state_ = 1;
      }
      else {
        state_ = 2;
      }
      
      return state_;
    }

    bool UpdateAvailable() {
      if (!initialized_) {
        Initialize();
      }
      //auto state = map(analogRead(pin_), 0, 4095, 0, num_states_-1);
      int val = analogRead(pin_);
      int state = 0;

      if (val >= 0 && val < 250) {
        state = 0;
      }
      else if (val >= 250 && val < 750) {
        state = 1;
      }
      else {
        state = 2;
      }
      
      return (state != state_);
    }
};

}
}

#endif //_TACTJAM_LIN_ENCODER_
