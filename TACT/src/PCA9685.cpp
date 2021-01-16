#include "PCA9685.h"
#include "config.h"

namespace tact  {

PCA9685::PCA9685() {
  *this = PCA9685(0x40, 800);
}


PCA9685::PCA9685(uint8_t address, float frequency) {
  address_ = address;
  frequency_ = frequency;
  pwm_driver_ = new Adafruit_PWMServoDriver(address_);
  active_positions_ = 0;
  initialized_ = false;
}


PCA9685::~PCA9685() {
  pwm_driver_->~Adafruit_PWMServoDriver();
  pwm_driver_ = nullptr;
}


void PCA9685::Initialize() {
  if (!Wire.busy()) {
    Wire.begin();
  }
  pwm_driver_->begin();
  pwm_driver_->setOscillatorFrequency(27000000);
  pwm_driver_->setPWMFreq(frequency_);
  Wire.setClock(400000);
  initialized_ = true;
}


void PCA9685::Update(uint8_t active_positions, uint16_t amplitude, bool enable_overdrive) {
  if (!initialized_) {
    Initialize();
  }

  if (active_positions == 0 || amplitude == 0) {
    for (uint8_t idx = 0; idx < 8; idx++) {
      pwm_driver_->setPWM(7-idx, 0, 0);
    }
    active_positions_ = active_positions;
    return;
  }

  if (enable_overdrive) {
    auto need_overdrive = false;
    for (uint8_t idx = 0; idx < 8; idx++) {
      uint8_t current_state = (active_positions >> idx) % 2;
      uint8_t last_state = (active_positions_ >> idx) % 2;
      if ((current_state == 1) && (current_state != last_state)) {
        pwm_driver_->setPWM(7-idx, 0, kMaxAmplitude);
        need_overdrive = true;
      }
    }

    if (need_overdrive) {
      // TODO delay may interfer with the control flow
      // find a better solution
      delay(config::kERMOverdriveDuration);
    }

    active_positions_ = active_positions;
  }
  
  for (uint8_t idx = 0; idx < 8; idx++) {
    if (((active_positions >> idx)%2) == 0) {
      pwm_driver_->setPWM(7-idx, 0, 0);
    } else {
      pwm_driver_->setPWM(7-idx, 0, amplitude);
    }
  }
}

}
