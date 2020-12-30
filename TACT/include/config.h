#ifndef _TACT_CONFIG_
#define _TACT_CONFIG_

#include <Arduino.h>

namespace tact {
namespace config {

  #ifdef TACT_DEBUG
  enum class DebugLevel {
    basic,
    verbose
  };
  const auto kDebugLevel = (TACT_DEBUG == 1) ? DebugLevel::basic : DebugLevel::verbose;
  #endif //TACT_DEBUG

  const auto kSerialBaudRate = 115200;
  const uint8_t kInitializationDelay = 20;


namespace esp {
namespace pins {
  const uint8_t kLinearEncoder = 15;
  const uint8_t kModeEncoder = 13;
  const uint8_t kSlotEncoder = 12;
  const uint8_t kM74HC166Latch = 33;
  const uint8_t kM74HC166Clock = 32;
  const uint8_t kM74HC166Data = 25;
  const uint8_t kSN74HC595Latch = 27;
  const uint8_t kSN74HC595Clock = 26;
  const uint8_t kSN74HC595Data = 14;
} //namespace pins
} //namespace esp
} //namespace config
} //namespace tact

#endif //_TACT_CONFIG_
