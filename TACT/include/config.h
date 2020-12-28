#ifndef _TACT_CONFIG_
#define _TACT_CONFIG_

namespace tact {
namespace config {

  const auto serial_baud_rate = 115200;

  #ifdef TACT_DEBUG
  enum class DebugLevel {
    basic,
    verbose
  };
  const auto debug_level = (TACT_DEBUG == 1) ? DebugLevel::basic : DebugLevel::verbose;
  #endif //TACT_DEBUG

  const uint8_t initialization_delay = 20;

  // ESP pin setup
  const uint8_t ESP_pin_linear_encoder = 15;
  const uint8_t ESP_pin_mode_encoder = 13;
  const uint8_t ESP_pin_slot_encoder = 12;
  const uint8_t ESP_pin_M74HC166_latch = 33;
  const uint8_t ESP_pin_M74HC166_clock = 32;
  const uint8_t ESP_pin_M74HC166_data = 25;
  const uint8_t ESP_pin_SN74HC595_latch = 27;
  const uint8_t ESP_pin_SN74HC595_clock = 26;
  const uint8_t ESP_pin_SN74HC595_data = 14;

}
}

#endif //_TACT_CONFIG_
