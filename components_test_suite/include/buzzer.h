#ifndef _TACTJAM_BUZZER_
#define _TACTJAM_BUZZER_

#define __TACTJAM_BUZZER_MULTIPLEXER__

#include <Arduino.h>

#ifdef __TACTJAM_BUZZER_MULTIPLEXER__
#include <Adafruit_PWMServoDriver.h>
#endif

namespace tactjam {

class Buzzer {
  private:
    #ifdef __TACTJAM_BUZZER_MULTIPLEXER__
    Adafruit_PWMServoDriver* PCA9685_;
    uint8_t pin_;
    uint8_t address_;
    float frequency_;
    bool initialized_;
    #else
    uint8_t pin_;
    uint8_t pwm_channel_;
    #endif
    

  public:
#ifdef __TACTJAM_BUZZER_MULTIPLEXER__
    Buzzer() {
      // default configuration
      pin_ = 8;
      address_ = 0x40;
      frequency_ = 800;
      PCA9685_ = new Adafruit_PWMServoDriver(address_);
    }

    ~Buzzer() {
      PCA9685_->~Adafruit_PWMServoDriver();
      PCA9685_ = nullptr;
    }

    void Initialize() {
      if (!Wire.busy()) {
        Wire.begin();
      }
      PCA9685_->begin();
      PCA9685_->setOscillatorFrequency(27000000);
      PCA9685_->setPWMFreq(frequency_);
      Wire.setClock(400000);
      initialized_ = true;
    }

    void NoTone(uint32_t length) {
      if (!initialized_) {
        Initialize();
      }
      PCA9685_->setPWM(pin_, 0, 0);
      delay(length);
    }

    void Tone(uint32_t length) { 
      if (!initialized_) {
        Initialize();
      }
      PCA9685_->setPWM(pin_, 0, 3000);
      delay(length);
      if (length>1) {
          delay(length);
          NoTone(0);
      }    
    }

    void TestMelody() {
      Tone(100);
      NoTone(50);
      Tone(100);
      NoTone(50);
      Tone(100);
      NoTone(50);
    }
#else
    Buzzer() {
      // default configuration
      *this = Buzzer(23, 0);
    }

    Buzzer(uint8_t pin, uint8_t pwm_channel) {
      pin_ = pin;
      pwm_channel_ = pwm_channel;
    }

    ~Buzzer() = default;

    void NoTone(uint32_t length) {
        ledcDetachPin(pin_);
        ledcWrite(pwm_channel_, 0);
        delay(length);
    }

    void Tone(double frequency, uint32_t length) {
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

    void TestMelody() {
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
#endif //__TACTJAM_BUZZER_MULTIPLEXER__
};

}

#endif //_TACTJAM_BUZZER_
