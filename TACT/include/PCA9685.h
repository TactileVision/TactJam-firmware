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

    /**
     * @brief Initialize the PWM driver.
     * 
     */
    void Initialize();

    /**
     * @brief Activate actuators with a given amplitude.
     * 
     * @param active_positions the bit pattern of activated actuators (1=activated).
     * @param amplitude the 12bit value (0-4095).
     */
    void Update(uint8_t active_positions, uint16_t amplitude = 4095);
};

}

#endif //_TACT_PCA9685_
