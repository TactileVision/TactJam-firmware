#include "M74HC166.h"
#include "config.h"

namespace tact {

M74HC166::M74HC166(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin) {
  latch_pin_ = latch_pin;
  clock_pin_ = clock_pin;
  data_pin_ = data_pin;
  initialized_ = false;
}


void M74HC166::Initialize() {
  pinMode(latch_pin_, OUTPUT);
  pinMode(clock_pin_, OUTPUT);
  pinMode(data_pin_, INPUT);
  data_ = 0;
  initialized_ = true;
}


uint16_t M74HC166::Read() {
  if (!initialized_) {
    Initialize();
  }
  digitalWrite(latch_pin_, LOW);
  digitalWrite(clock_pin_, LOW);
  digitalWrite(clock_pin_, HIGH);
  digitalWrite(latch_pin_, HIGH);
  data_ = 0;
  for (uint8_t j = 0; j < 16; j++) {
    uint16_t d = digitalRead(data_pin_);
    if (d == 1) {
      bitSet(data_, j);
    }
    digitalWrite(clock_pin_, LOW);
    digitalWrite(clock_pin_, HIGH);
  }
  return data_;
}

}
