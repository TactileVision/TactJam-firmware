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

const uint8_t kERMOverdriveDuration = 20;

// The dev-board v0.2 has the buzzer attached to the PWM multiplexer.
// TODO: remove definition for final board
#define __TACT_BUZZER_MULTIPLEXER__
#ifdef __TACT_BUZZER_MULTIPLEXER__
const uint8_t kBuzzerID = 8;
#endif

#if TACT_BOARD_REV==0
const uint8_t kActuatorMapping[] = {0,1,2,3,4,5,6,7};
#elif TACT_BOARD_REV==1
const uint8_t kActuatorMapping[] = {0,1,2,3,8,9,10,11};
#endif //TACT_BOARD_REV

namespace esp {
namespace pins {

#if TACT_BOARD_REV==0
const uint8_t kLinearEncoder  = 15;
const uint8_t kModeEncoder    = 13;
const uint8_t kSlotEncoder    = 12;
const uint8_t kM74HC166Latch  = 33;
const uint8_t kM74HC166Clock  = 32;
const uint8_t kM74HC166Data   = 25;
const uint8_t kSN74HC595Latch = 27;
const uint8_t kSN74HC595Clock = 26;
const uint8_t kSN74HC595Data  = 14;
const uint8_t kBuzzer         = 23;
#elif TACT_BOARD_REV==1
const uint8_t kLinearEncoder  = 34;
const uint8_t kModeEncoder    = 35;
const uint8_t kSlotEncoder    = 36;
const uint8_t kM74HC166Latch  = 33;
const uint8_t kM74HC166Clock  = 32;
const uint8_t kM74HC166Data   = 25;
const uint8_t kSN74HC595Latch = 27;
const uint8_t kSN74HC595Clock = 26;
const uint8_t kSN74HC595Data  = 16;
const uint8_t kBuzzer         = 4;
#endif //TACT_BOARD_REV

} //namespace pins

inline void DisableRadios() {
  if (btStarted()) {
    btStop();
  }
}

} //namespace esp
} //namespace config
} //namespace tact

#endif //_TACT_CONFIG_
