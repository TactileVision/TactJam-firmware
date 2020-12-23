#include "display.h"

namespace tact {

Display::Display() {
  width_ = 128;
  height_ = 64;
  reset_pin_ = 4;
  address_ = 0x3C;
  initialized_ = false;
  SSD1306_ = new Adafruit_SSD1306(width_, height_, &Wire, reset_pin_);
}


Display::~Display() {
  SSD1306_->~Adafruit_SSD1306();
  SSD1306_ = nullptr;
}


bool Display::Initialize() {
  initialized_ = SSD1306_->begin(SSD1306_SWITCHCAPVCC, address_);
  return initialized_;
}


void Display::DrawBootScreen() {
  // lazy evaltuation should prevent call to Initialize if already initialized
  if (!initialized_ && !Initialize()) {
    return;
  }
  int16_t x = 20;
  int16_t y = 10;
  SSD1306_->clearDisplay();
  SSD1306_->invertDisplay(false);
  SSD1306_->setTextColor(SSD1306_WHITE);
  SSD1306_->setTextSize(2);
  SSD1306_->setCursor(x, y);
  SSD1306_->println("TactJam");
  SSD1306_->setTextSize(1);
  SSD1306_->setCursor(x, 4*y);
  SSD1306_->print("version ");
  SSD1306_->println(TACT_VERSION);
  SSD1306_->setCursor(x, 5*y);
  SSD1306_->print("rev ");
  SSD1306_->println(GIT_REV);
  SSD1306_->display();
}


void Display::DrawMenuScreen() {
  // lazy evaltuation should prevent call to Initialize if already initialized
  if (!initialized_ && !Initialize()) {
    return;
  }
  SSD1306_->clearDisplay();
  SSD1306_->invertDisplay(false);
  DrawStatusBar();
  SSD1306_->drawRect(0, 12, 128, 64-12, SSD1306_WHITE);
  SSD1306_->setTextColor(SSD1306_WHITE);
  SSD1306_->setTextSize(1);
  SSD1306_->setCursor(4, 54);
  SSD1306_->print("v");
  SSD1306_->println(TACT_VERSION);
  SSD1306_->display();
}


void Display::DrawStatusBar() {
  DrawMenuPair("mode", "jam", 0, 0);
  DrawMenuPair("slot", "1", 52, 0);
  DrawMenuPair("a", "100%", 92, 0);
}


void Display::UpdateModeSelection(const String& mode) {
  auto title_width = 4 * kCharWidth;
  auto title_line_width = title_width + kTextOffsetLeft + kTextOffsetRight;
  DrawMenuValue(mode, title_line_width, 0);
  SSD1306_->display();
}


void Display::UpdateSlotSelection(uint8_t slot) {
  auto title_width = 4 * kCharWidth;
  auto title_line_width = title_width + kTextOffsetLeft + kTextOffsetRight;
  DrawMenuValue(String(slot), 52 + title_line_width, 0);
  SSD1306_->display();
}


void Display::UpdateAmplitude(uint8_t amplitude_perc) {
  auto title_width = kCharWidth;
  auto title_line_width = title_width + kTextOffsetLeft + kTextOffsetRight;
  DrawMenuValue(String(amplitude_perc)+"%", 92 + title_line_width, 0);
  SSD1306_->display();
}


void Display::DrawMenuPair(const String& title, const String& value, int16_t x, int16_t y) {
  SSD1306_->setTextSize(1);
  // title column
  auto title_width = title.length() * kCharWidth;
  auto title_line_width = title_width + kTextOffsetLeft + kTextOffsetRight;
  SSD1306_->drawRect(x, y, title_line_width, kLineHeight, SSD1306_WHITE);
  SSD1306_->setTextColor(SSD1306_WHITE);
  SSD1306_->setCursor(x + kTextOffsetLeft, y + kTextOffsetTop);
  SSD1306_->println(title);
  // value column
  DrawMenuValue(value, x + title_line_width, y);
}


void Display::DrawMenuValue(const String& value, int16_t x, int16_t y) {
  auto value_width = value.length() * kCharWidth;
  auto value_line_width = value_width + kTextOffsetLeft + kTextOffsetRight;
  SSD1306_->fillRect(x, y, value_line_width, kLineHeight, SSD1306_WHITE);
  SSD1306_->setTextColor(SSD1306_BLACK);
  SSD1306_->setCursor(x + kSpacing, y + kTextOffsetTop);
  SSD1306_->println(value);
}

}
