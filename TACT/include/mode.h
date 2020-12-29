#ifndef _TACT_MODE_
#define _TACT_MODE_

#include <Arduino.h>

namespace tact {

enum class Modes : uint8_t {
  undefined = 0,
  jam = 1,
  rec_play = 2,
  transfer = 3
};

struct Mode {
    /**
     * @brief Get the name of a given mode.
     * 
     * @param mode the selected mode.
     * @return String the name of the mode.
     */
    static String GetName(tact::Modes mode) {
      String name;
      switch (mode) {
      case Modes::undefined:
        name = "---";
        break;
      case Modes::jam:
        name = "jam";
        break;
      case Modes::rec_play:
        name = "r/p";
        break;
      case Modes::transfer:
        name = "dat";
        break;
      }
      return name;
    }
};

}

#endif //_TACT_MODE_
