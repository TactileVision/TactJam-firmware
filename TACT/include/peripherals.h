#ifndef _TACT_PERIPHERALS_
#define _TACT_PERIPHERALS_

#include "config.h"
#include "mode.h"
#include "state.h"
#include "display.h"
#include "linearEncoder.h"
#include "rotarySwitch3Pos.h"
#include "M74HC166.h"
#include "SN74HC595.h"
#include "PCA9685.h"
#include "buzzer.h"


namespace tact {

struct Peripherals {
  Display display;
  LinearEncoder amplitude_encoder{config::esp::pins::kLinearEncoder};
  RotarySwitch3Pos mode_encoder{config::esp::pins::kModeEncoder};
  RotarySwitch3Pos slot_encoder{config::esp::pins::kSlotEncoder};
  M74HC166 buttons{
      config::esp::pins::kM74HC166Latch,
      config::esp::pins::kM74HC166Clock,
      config::esp::pins::kM74HC166Data
  };
  SN74HC595 button_leds{
      config::esp::pins::kSN74HC595Latch,
      config::esp::pins::kSN74HC595Clock,
      config::esp::pins::kSN74HC595Data
  };
  PCA9685 actuator_driver;
  #ifdef __TACT_BUZZER_MULTIPLEXER__
  Buzzer buzzer;
  #else
  Buzzer buzzer{config::esp::pins::kBuzzer};
  #endif

  void Initialize() {
    amplitude_encoder.Initialize();
    delay(tact::config::kInitializationDelay);
    mode_encoder.Initialize();
    delay(tact::config::kInitializationDelay);
    slot_encoder.Initialize();
    delay(tact::config::kInitializationDelay);
    buttons.Initialize();
    delay(tact::config::kInitializationDelay);
    button_leds.Initialize();
    delay(tact::config::kInitializationDelay);
    button_leds.Update(0);
    delay(tact::config::kInitializationDelay);
    actuator_driver.Initialize();
    delay(tact::config::kInitializationDelay);
    actuator_driver.Update(0, 0);
    delay(tact::config::kInitializationDelay);
    buzzer.Initialize();
    delay(tact::config::kInitializationDelay);
    if (!display.Initialize()) {
      #ifdef TACT_DEBUG
      Serial.println("ERROR: display setup");
      #endif //TACT_DEBUG
    }
  }
};

}

#endif //_TACT_PERIPHERALS_