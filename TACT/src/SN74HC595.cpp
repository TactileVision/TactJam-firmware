#include "SN74HC595.h"

namespace tact {

SN74HC595::SN74HC595(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin, uint8_t bit_order) {
  latch_pin_ = latch_pin;
  clock_pin_ = clock_pin;
  data_pin_ = data_pin;
  bit_order_ = bit_order;
  initialized_ = false;
}


void SN74HC595::Initialize() {
  pinMode(latch_pin_, OUTPUT);
  pinMode(clock_pin_, OUTPUT);
  pinMode(data_pin_, OUTPUT);
  initialized_ = true;
}


void SN74HC595::Update(uint8_t active_positions) {
  if (!initialized_) {
    Initialize();
  }
  digitalWrite(latch_pin_, LOW);
  shiftOut(data_pin_, clock_pin_, bit_order_, active_positions);
  digitalWrite(latch_pin_, HIGH);
}

} 
