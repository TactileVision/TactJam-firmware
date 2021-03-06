#ifndef _TACT_CONFIG_
#define _TACT_CONFIG_

#include <Arduino.h>
#include <Wire.h>
#include "debug.h"

namespace tact {
namespace config {

const auto kSerialBaudRate = 115200;
const uint8_t kInitializationDelay = 20;

const uint8_t kERMOverdriveDuration = 20;

// The dev-board v0.2 has the buzzer attached to the PWM multiplexer.
#if TACT_BOARD_REV==0
#define __TACT_BUZZER_MULTIPLEXER__
const uint8_t kBuzzerID = 8;
#endif //TACT_BOARD_REV

#if TACT_BOARD_REV==0
const uint8_t kActuatorMapping[] = {0,1,2,3,4,5,6,7};
#elif TACT_BOARD_REV==1
const uint8_t kActuatorMapping[] = {0,1,2,3,8,9,10,11};
#endif //TACT_BOARD_REV

namespace esp {
namespace pins {

const uint8_t kI2CSCL  = 22;
const uint8_t kI2CSDA  = 21;

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

inline void StartI2C() {
  if (!Wire.busy()) {
    // run I2C in fast mode (400kHz)
    Wire.begin(pins::kI2CSDA, pins::kI2CSCL, 400000U);
    
  }
  #ifdef TACT_DEBUG
  debug::debug_stream << "start I2C bus with " << Wire.getClock() << "Hz";
  debug::println(debug::debug_stream.str());
  #endif //TACT_DEBUG
}

} //namespace esp
} //namespace config
} //namespace tact

#endif //_TACT_CONFIG_
