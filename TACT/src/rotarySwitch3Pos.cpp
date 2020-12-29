#include "rotarySwitch3Pos.h"

namespace tact {

RotarySwitch3Pos::RotarySwitch3Pos(uint8_t pin) {
  pin_ = pin;
  initialized_ = false;
}


void RotarySwitch3Pos::Initialize() {
  pinMode(pin_, INPUT);
  position_ = Read();
  initialized_ = true;
}


uint8_t RotarySwitch3Pos::GetPosition() {
  if (!initialized_) {
    Initialize();
  }
  position_ = Read();
  return position_;
}


bool RotarySwitch3Pos::UpdateAvailable() {
  if (!initialized_) {
    Initialize();
  }
  uint8_t pos = Read();
  return (pos != position_);
}


uint8_t RotarySwitch3Pos::Read() {
  int val = analogRead(pin_);
  int pos = 0;
  if (val >= kBoundPos0 && val < kBoundPos1) {
    pos = 1;
  }
  else if (val >= kBoundPos1 && val < kBoundPos2) {
    pos = 2;
  }
  else {
    pos = 3;
  }
  return pos;
}

}
