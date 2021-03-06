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
  initialized_ = false;
}


PCA9685::~PCA9685() {
  pwm_driver_->~Adafruit_PWMServoDriver();
  pwm_driver_ = nullptr;
}


void PCA9685::Initialize() {
  pwm_driver_->begin();
  pwm_driver_->setOscillatorFrequency(27000000);
  pwm_driver_->setPWMFreq(frequency_);
  initialized_ = true;
  active_positions_old = 0;
  delay(config::kInitializationDelay);
  for (uint8_t pin=0; pin<16; pin++) {
    pwm_driver_->setPWM(pin, 0, 0);
  }
  delay(config::kInitializationDelay);
}


void PCA9685::Update(uint8_t active_positions, uint16_t amplitude, bool enable_overdrive) {
  if (!initialized_) {
    Initialize();
  }
  if (active_positions == 0 || amplitude == 0) {
    for (uint8_t idx = 0; idx < 16; idx++) {
      pwm_driver_->setPWM(idx, 0, 0);
    }
    active_positions_old = active_positions;
    return;
  }
  if (enable_overdrive) {
    bool overdrive_activated = false;
    for (uint8_t idx = 0; idx < 8; idx++) {
      if ((((active_positions_old >> idx)%2) == 0) &&
         ((active_positions >> idx)%2) == 1) {
        pwm_driver_->setPWM(config::kActuatorMapping[7-idx], 0, kMaxAmplitude);
        overdrive_activated = true;
      }
    }
    // TODO delay may interfer with the control flow
    // find a better solution
    if (overdrive_activated == true) {
      delay(config::kERMOverdriveDuration);
    }
  }
  for (uint8_t idx = 0; idx < 8; idx++) {
    if (((active_positions >> idx)%2) == 0) {
      pwm_driver_->setPWM(config::kActuatorMapping[7-idx], 0, 0);
    } else {
      pwm_driver_->setPWM(config::kActuatorMapping[7-idx], 0, amplitude);
    }
  }
  active_positions_old = active_positions;
}

}
