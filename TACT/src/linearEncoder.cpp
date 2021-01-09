#include "linearEncoder.h"

namespace tact {

LinearEncoder::LinearEncoder(uint8_t pin, uint16_t filter_width) {
  pin_ = pin;
  filter_width_ = filter_width;
  initialized_ = false;
}


void LinearEncoder::Initialize() {
  pinMode(pin_, INPUT);
  value_ = 0;
  initialized_ = true;
  Read();
}


bool LinearEncoder::UpdateAvailable() {
  auto val = analogRead(pin_);
  auto lower = (value_-filter_width_ > 0) ? value_-filter_width_ : 0;
  auto upper = (value_+filter_width_ < 4096) ? value_+filter_width_ : 4095;
  return ((val < lower) || (val > upper));
}


uint16_t LinearEncoder::Read() {
  if (!initialized_) {
    Initialize();
  }
  value_ = analogRead(pin_);
  return value_;
}


uint8_t LinearEncoder::GetPercent(bool update) {
  if (update) {
    Read();
  }
  return (uint8_t)map(value_, 0, 4095, 0, 100);
}


uint16_t LinearEncoder::PercentToLinearEncoder(uint8_t percent) {
  return map(percent, 0, 100, 0, 4095);
}


uint8_t LinearEncoder::Get8bit(bool update) {
  if (update) {
    Read();
  }
  return (uint8_t)map(value_, 0, 4095, 0, 255);
}


uint16_t LinearEncoder::Get12bit(bool update) {
  if (update) {
    Read();
  }
  return value_;
}

}
