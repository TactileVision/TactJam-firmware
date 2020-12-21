#ifndef _TACTJAM_I2CSCAN_
#define _TACTJAM_I2CSCAN_

#include <Arduino.h>
#include <Wire.h>

namespace tactjam {
namespace i2c {


class Scanner {
  private:
    std::vector<uint8_t> devices_;
    bool initialized_ = false;

  public:
    Scanner() = default;

    ~Scanner() {
      devices_.clear();
    }

    void Initialize() {
      if (!Wire.busy()) {
        Wire.begin();
      }
      initialized_ = true;
    }

    void Scan(bool print = true) {
      devices_.clear();
      for (uint8_t address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
          devices_.push_back(address);
        }
      }
      if (print) {
        Print();
      }
    }

    void Print() {
      if (devices_.empty()) {
        Serial.println("No devices available.");
        return;
      }
      Serial.printf("%d devices available: ", devices_.size());
      for (auto address : devices_) {
        Serial.print("\t0x");
        Serial.print(address, HEX);
      }
      Serial.println();
    }
};

}
}

#endif // _TACTJAM_I2CSCAN_