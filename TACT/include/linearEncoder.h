#ifndef _TACT_LINEAR_ENCODER_
#define _TACT_LINEAR_ENCODER_

#include <Arduino.h>

namespace tact {

class LinearEncoder {
  private:
    uint8_t pin_;
    uint16_t value_;
    uint16_t filter_width_;
    bool initialized_;

  public:
    LinearEncoder() = delete;
    LinearEncoder(uint8_t pin, uint16_t filter_width = 30);
    ~LinearEncoder() = default;

    /**
     * @brief Initialize the encoder.
     * 
     */
    void Initialize();

    /**
     * @brief Check if the encoder was turned.
     * 
     * @return true a new measurement is available.
     * @return false the encoder is still in the same position.
     */
    bool UpdateAvailable();

    /**
     * @brief Read the analog value (resistance) of the encoder.
     * 
     * @return uint16_t 12bit value (0-4095).
     */
    uint16_t Read();

    /**
     * @brief Get the amplitude in percent (0–100).
     * 
     * @param update If true a new measurement is taken before returning the value.
     * @return uint8_t the amplitude in percent (0-100).
     */
    uint8_t GetPercent(bool update = true);

    /**
     * @brief  Get the amplitude as 8bit value (0-255).
     * 
     * @param update If true a new measurement is taken before returning the value.
     * @return uint8_t 8bit value (0-255).
     */
    uint8_t Get8bit(bool update = true);

    /**
     * @brief Get the amplitude as 12bit value (0-4095)
     * 
     * @param update If true a new measurement is taken before returning the value.
     * @return uint16_t 12bit value (0-4095).
     */
    uint16_t Get12bit(bool update = true);
};

}

#endif //_TACT_LINEAR_ENCODER_
