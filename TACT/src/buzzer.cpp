#include "buzzer.h"

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
  if (length>1) {
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

#else

Buzzer::Buzzer(uint8_t pin, uint8_t pwm_channel) {
  pin_ = pin;
  pwm_channel_ = pwm_channel;
  initialized_ = false;
}


void Buzzer::Initialize() {
  initialized_ = true;
}


void Buzzer::NoTone(uint32_t length) {
  ledcDetachPin(pin_);
  ledcWrite(pwm_channel_, 0);
  delay(length);
}


void Buzzer::Tone(double frequency, uint32_t length) {
  if (ledcRead(pwm_channel_)) {
      return;
  }
  ledcAttachPin(pin_, pwm_channel_);
  ledcWriteTone(pwm_channel_, frequency);
  if (length>1) {
      delay(length);
      NoTone(0);
  }    
}


void Buzzer::PlayInitSequence() {
  Tone(700, 100);
  NoTone(50);
  Tone(600, 100);
  NoTone(50);
  Tone(500, 100);
  NoTone(50);
  Tone(400, 100);
  NoTone(50);
  Tone(300, 100);
  NoTone(50);
  Tone(500, 300);
  NoTone(100);
  Tone(500, 300);
  NoTone(100);
  Tone(500, 300);
  NoTone(100);
}
#endif //__TACT_BUZZER_MULTIPLEXER__

}
