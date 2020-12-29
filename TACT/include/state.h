#ifndef _TACT_STATE_
#define _TACT_STATE_

#include "mode.h"

namespace tact {

struct State {
  Modes mode = Modes::undefined;
  uint8_t slot = 0;
  uint16_t pressed_buttons = 0;
  uint8_t pressed_actuator_buttons = 0;
  uint8_t pressed_menu_buttons = 0;
  uint16_t amplitude = 0;
  uint8_t amplitude_percent = 0;
};
  
}

#endif //_TACT_STATE_
