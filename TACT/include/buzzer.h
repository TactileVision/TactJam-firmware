#ifndef _TACT_BUZZER_
#define _TACT_BUZZER_

#include <Arduino.h>
#include "config.h"

#ifdef __TACT_BUZZER_MULTIPLEXER__
#include <Adafruit_PWMServoDriver.h>
#endif

namespace tact {

class Buzzer {
  private:
    #ifdef __TACT_BUZZER_MULTIPLEXER__
    Adafruit_PWMServoDriver* PCA9685_;
    uint8_t address_;
    float frequency_;
    #else
    uint8_t pwm_channel_;
    #endif
    uint8_t pin_;
    bool initialized_;
    
  public:
#ifdef __TACT_BUZZER_MULTIPLEXER__
    Buzzer();
    ~Buzzer();

    /**
     * @brief 
     * 
     */
    void Initialize();

    /**
     * @brief 
     * 
     * @param length 
     */
    void NoTone(uint32_t length);

    /**
     * @brief 
     * 
     * @param length 
     */
    void Tone(uint32_t length);

    /**
     * @brief 
     * 
     */
    void PlayInitSequence();
#else
    Buzzer() = delete;
    Buzzer(uint8_t pin, uint8_t pwm_channel = 0);
    ~Buzzer() = default;

    /**
     * @brief 
     * 
     */
    void Initialize();

    /**
     * @brief 
     * 
     * @param length 
     */
    void NoTone(uint32_t length);

    /**
     * @brief 
     * 
     * @param frequency 
     * @param length 
     */
    void Tone(double frequency, uint32_t length);

    /**
     * @brief 
     * 
     */
    void PlayInitSequence();
#endif //__TACT_BUZZER_MULTIPLEXER__
};

}

#endif //_TACT_BUZZER_
