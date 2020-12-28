#ifndef _TACT_PCA9685_
#define _TACT_PCA9685_

#include <Adafruit_PWMServoDriver.h>

namespace tact  {
class PCA9685 {
  private:
    Adafruit_PWMServoDriver* pwm_driver_;
    uint8_t address_;
    float frequency_;
    bool initialized_;

  public:
    PCA9685();
    PCA9685(uint8_t address, float frequency);
    ~PCA9685();

    void Initialize();
    void Update(uint8_t active_positions, uint16_t amplitude = 4095);
};

}

#endif //_TACT_PCA9685_
