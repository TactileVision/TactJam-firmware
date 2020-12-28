#include "PCA9685.h"

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
  if (!Wire.busy()) {
    Wire.begin();
  }
  pwm_driver_->begin();
  pwm_driver_->setOscillatorFrequency(27000000);
  pwm_driver_->setPWMFreq(frequency_);
  Wire.setClock(400000);
  initialized_ = true;
}


void PCA9685::Update(uint8_t active_positions, uint16_t amplitude) {
  if (!initialized_) {
    Initialize();
  }
  for (uint8_t idx = 0; idx < 8; idx++) {
    if (((active_positions >> idx)%2) == 0) {
      pwm_driver_->setPWM(7-idx, 4095, 0);
    } else {
      pwm_driver_->setPWM(7-idx, 0, amplitude);
    }
  }
}

}