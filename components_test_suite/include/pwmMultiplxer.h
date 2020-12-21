#ifndef _TACTJAM_PWMMULTIPLEXER_
#define _TACTJAM_PWMMULTIPLEXER_

// un-/comment to switch driver library
#define _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_

#include <Wire.h>
#ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
#include <Adafruit_PWMServoDriver.h>
#else
#include <PCA9685.h>
#endif

namespace tactjam {
namespace pwm {

class PWMPCA9685 {
  private:
    #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
    Adafruit_PWMServoDriver* PCA9685_;
    #else
    PCA9685* PCA9685_;
    uint8_t drive_mode_;
    #endif
    uint8_t address_;
    float frequency_;
    bool initialized_;

  public:
    PWMPCA9685() {
      #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
      *this = PWMPCA9685(0x40, 800);
      #else
      *this = PWMPCA9685(0x40, PCA9685_MODE_LED_DIRECT, 800);
      #endif
    }

    #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
    PWMPCA9685(uint8_t address, float frequency) {
      address_ = address;
      frequency_ = frequency;
      PCA9685_ = new Adafruit_PWMServoDriver(address_);
      initialized_ = false;
    }  
    #else
    PWMPCA9685(uint8_t address, uint8_t drive_mode, float frequency) {
      address_ = address;
      drive_mode_ = drive_mode;
      frequency_ = frequency;
      PCA9685_ = new PCA9685(address_, drive_mode_, frequency_);
      initialized_ = false;
    }   
    #endif

    ~PWMPCA9685() {
      #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
      PCA9685_->~Adafruit_PWMServoDriver();
      #else
      PCA9685_->~PCA9685();
      #endif
      PCA9685_ = nullptr;
    }

    void Initialize() {
      if (!Wire.busy()) {
        Wire.begin();
      }
      #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
      PCA9685_->begin();
      PCA9685_->setOscillatorFrequency(27000000);
      PCA9685_->setPWMFreq(frequency_);
      Wire.setClock(400000);
      #else
      PCA9685_->setup();
      #endif
      initialized_ = true;
    }

    void Test() {
      if (!initialized_) {
        Initialize();
      }
      // blink
      for (uint8_t i = 0; i < 4; i++) {
        // turn on
        for (uint8_t idx = 0; idx < 8; idx++) {
          #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
          PCA9685_->setPWM(idx, 0, 4095);
          #else
          PCA9685_->getPin(idx).fullOnAndWrite();
          #endif
        }
        delay(500);
        // turn off
        for (uint8_t idx = 0; idx < 8; idx++) {
          #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
          PCA9685_->setPWM(idx, 4095, 0);
          #else
          PCA9685_->getPin(idx).fullOffAndWrite();
          #endif
        }
        delay(500);
      }

      // wave
      for (uint16_t i=0; i<4096; i += 8) {
        for (uint8_t idx=0; idx < 8; idx++) {
          #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
          PCA9685_->setPWM(idx, 0, (i + (4096/16)*idx) % 4096);
          #else
          PCA9685_->getPin(idx).setValueAndWrite((i + (4096/8)*idx) % 4096);
          #endif
        }
      }

      // turn off
      for (uint8_t idx = 0; idx < 8; idx++) {
        #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
        PCA9685_->setPWM(idx, 4095, 0);
        #else
        PCA9685_->getPin(idx).fullOffAndWrite();
        #endif
      }
    }

    void Update(uint8_t activations, uint16_t amplitude = 4095) {
      if (!initialized_) {
        Initialize();
      }
      for (uint8_t idx = 0; idx < 8; idx++) {
        if (((activations >> idx)%2) == 0) {
          #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
          PCA9685_->setPWM(7-idx, 4095, 0);
          #else
          PCA9685_->getPin(idx).fullOnAndWrite();
          #endif
        } else {
          #ifdef _TACTJAM_PWMMULTIPLEXER_ADAFRUIT_
          PCA9685_->setPWM(7-idx, 0, amplitude);
          #else
          PCA9685_->getPin(idx).fullOffAndWrite();
          #endif
        }
      }
    }
};

}
}

#endif // _TACTJAM_PWMMULTIPLEXER_
