#ifndef _TACT_MODE_
#define _TACT_MODE_

#include <Arduino.h>

namespace tact {

enum class Modes : uint8_t {
  undefined = 0,
  jam = 1,
  record = 2,
  transfer = 3
};

class Mode {
  private:
    Mode() {}

  public:
    static String GetName(tact::Modes mode) {
      String name;
      switch (mode) {
      case Modes::undefined:
        name = "---";
        break;
      case Modes::jam:
        name = "jam";
        break;
      case Modes::record:
        name = "rec";
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
