#include "buzzer.h"

#ifndef __TACT_BUZZER_MULTIPLEXER__
#include <Tone32.h>
#endif

namespace tact {

#ifdef __TACT_BUZZER_MULTIPLEXER__
Buzzer::Buzzer() {
  // default configuration
  pin_ = config::kBuzzerID;
  address_ = 0x40;
  frequency_ = 800;
  PCA9685_ = new Adafruit_PWMServoDriver(address_);
}


Buzzer::~Buzzer() {
  PCA9685_->~Adafruit_PWMServoDriver();
  PCA9685_ = nullptr;
}


void Buzzer::Initialize() {
  if (!Wire.busy()) {
    Wire.begin();
  }
  PCA9685_->begin();
  PCA9685_->setOscillatorFrequency(27000000);
  PCA9685_->setPWMFreq(frequency_);
  Wire.setClock(400000);
  initialized_ = true;
}


void Buzzer::NoTone(uint32_t length) {
  if (!initialized_) {
    Initialize();
  }
  PCA9685_->setPWM(pin_, 0, 0);
  delay(length);
}


void Buzzer::Tone(uint32_t length) { 
  if (!initialized_) {
    Initialize();
  }
  PCA9685_->setPWM(pin_, 0, 3000);
  if (length > 1) {
    delay(length);
    NoTone(0);
  }    
}


void Buzzer::PlayInitSequence() {
  Tone(100);
  NoTone(50);
  Tone(100);
  NoTone(50);
  Tone(100);
  NoTone(50);
}

void Buzzer::PlayConfirm() {
  Tone(50);
  NoTone(10);
}

void Buzzer::PlaySuccess() {
  Tone(50);
  NoTone(20);
  Tone(50);
  NoTone(20);
}

void Buzzer::PlayFail() {
  Tone(100);
  NoTone(20);
  Tone(200);
  NoTone(20);
}

#else

Buzzer::Buzzer(uint8_t pin, uint8_t pwm_channel) {
  pin_ = pin;
  pwm_channel_ = pwm_channel;
  initialized_ = false;
}


void Buzzer::Initialize() {
  initialized_ = true;
}


void Buzzer::PlayInitSequence() {
  tone(pin_, NOTE_C4, 60, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_E4, 60, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_G4, 60, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_B4, 60, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_C5, 60, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_E5, 80, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_G5, 100, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_B5, 150, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_B5, 200, pwm_channel_);
  noTone(pin_, pwm_channel_);
}


void Buzzer::PlayConfirm() {
  tone(pin_, NOTE_C4, 100, pwm_channel_);
  noTone(pin_, pwm_channel_);
}


void Buzzer::PlayFail() {
  tone(pin_, NOTE_C3, 100, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_C2, 200, pwm_channel_);
  noTone(pin_, pwm_channel_);
}


void Buzzer::PlaySuccess() {
  tone(pin_, NOTE_C4, 100, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_E4, 100, pwm_channel_);
  noTone(pin_, pwm_channel_);
  tone(pin_, NOTE_G4, 100, pwm_channel_);
  noTone(pin_, pwm_channel_);
}
#endif //__TACT_BUZZER_MULTIPLEXER__

}
