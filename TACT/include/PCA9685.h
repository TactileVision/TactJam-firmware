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
    uint8_t active_positions_old;

  public:
    PCA9685();
    PCA9685(uint8_t address, float frequency);
    ~PCA9685();

    static const uint16_t kMaxAmplitude = 4095;

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
     * @param enable_overdrive if set to true the actuator will start on max. amplitude for a short period of time
     */
    void Update(uint8_t active_positions, uint16_t amplitude = kMaxAmplitude, bool enable_overdrive = true);
};

}

#endif //_TACT_PCA9685_
