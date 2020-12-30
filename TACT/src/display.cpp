#include "display.h"

namespace tact {

Display::Display() {
  initialized_ = false;
  SSD1306_ = new Adafruit_SSD1306(kWidth, kHeight, &Wire, kResetPin);
}


Display::~Display() {
  SSD1306_->~Adafruit_SSD1306();
  SSD1306_ = nullptr;
}


bool Display::Initialize() {
  initialized_ = SSD1306_->begin(SSD1306_SWITCHCAPVCC, kAddress);
  return initialized_;
}


void Display::DrawBootScreen() {
  if (!initialized_ && !Initialize()) {
    return;
  }
  SSD1306_->clearDisplay();
  SSD1306_->invertDisplay(false);
  SSD1306_->setTextColor(SSD1306_WHITE);
  SSD1306_->setTextSize(2);
  SSD1306_->setCursor(kBootScreenTextX, kLineHeight);
  SSD1306_->println("TactJam");
  SSD1306_->setTextSize(1);
  SSD1306_->setCursor(kBootScreenTextX, 4*kLineHeight);
  SSD1306_->print("version ");
  SSD1306_->println(TACT_VERSION);
  SSD1306_->setCursor(kBootScreenTextX, 5*kLineHeight);
  SSD1306_->print("rev ");
  SSD1306_->println(GIT_REV);
  SSD1306_->display();
}


void Display::DrawMenuScreen(const String& mode, const uint8_t slot, const uint8_t amplitude) {
  if (!initialized_ && !Initialize()) {
    return;
  }
  SSD1306_->clearDisplay();
  SSD1306_->invertDisplay(false);
  UpdateStatusBar(mode, slot, amplitude);
  DrawContentBorder();
  ClearContent();
  SSD1306_->display();
}


void Display::DrawModeSelection(const String& mode) {
  if (!initialized_ && !Initialize()) {
    return;
  }
  UpdateStatusValue(mode, kModeValueX, kStatusBarY, kModeValueWidth);
  SSD1306_->display();
}


void Display::DrawSlotSelection(uint8_t slot) {
  if (!initialized_ && !Initialize()) {
    return;
  }
  UpdateStatusValue(String(slot), kSlotValueX, kStatusBarY, kSlotValueWidth);
  SSD1306_->display();
}


void Display::DrawAmplitude(uint8_t amplitude_perc) {
  if (!initialized_ && !Initialize()) {
    return;
  }
  UpdateStatusValue(String(amplitude_perc)+"%", kAmplitudeValueX, kStatusBarY, kAmplitudeValueWidth);
  SSD1306_->display();
}


void Display::DrawContentTeaser(const String& text) {
  if (!initialized_ && !Initialize()) {
    return;
  }
  ClearContent();
  SSD1306_->invertDisplay(false);
  SSD1306_->setTextColor(SSD1306_WHITE);
  SSD1306_->setTextSize(2);
  int16_t x = kContentSpacing;
  auto text_width = text.length() * kCharWidth * 2;
  if (text_width < kContentWidth) {
    x = (kWidth - text_width) / 2;
  }
  SSD1306_->setCursor(x, kContentOffsetTop);
  SSD1306_->println(text);
  SSD1306_->display();
}

// TODO: This method declaration may change depending on the information we need to display.
void Display::DrawTactonDetails(const uint8_t slot, const String& uuid, uint32_t instruction_size, uint64_t length_millis) {
  ClearContent();
  SSD1306_->setTextColor(SSD1306_WHITE);
  SSD1306_->setTextSize(1);
  SSD1306_->setCursor(kContentSpacing, kContentOffsetTop);
  SSD1306_->printf("slot:         %u\n", slot);
  SSD1306_->setCursor(kContentSpacing, SSD1306_->getCursorY()+kSpacing);
  SSD1306_->printf("uuid:         %s\n", uuid.c_str());
  SSD1306_->setCursor(kContentSpacing, SSD1306_->getCursorY()+kSpacing);
  SSD1306_->printf("instructions: %u\n", instruction_size);
  SSD1306_->setCursor(kContentSpacing, SSD1306_->getCursorY()+kSpacing);
  SSD1306_->printf("length (ms):  %u\n", (uint32_t)length_millis);
  SSD1306_->setCursor(kContentSpacing, SSD1306_->getCursorY()+kSpacing);
  SSD1306_->printf("length (s):   %u\n", (uint32_t)(length_millis/1000));
}


void Display::UpdateStatusBar(const String& mode, const uint8_t slot, const uint8_t amplitude) {
  UpdateStatusPair(kModeTitle, kModeTitleWidth, mode, kModeValueWidth, kModeTitleX, kStatusBarY);
  UpdateStatusPair(kSlotTitle, kSlotTitleWidth, String(slot), kSlotValueWidth, kSlotTitleX, kStatusBarY);
  UpdateStatusPair(kAmplitudeTitle, kAmplitudeTitleWidth, String(amplitude) + "%", kAmplitudeValueWidth, kAmplitudeTitleX, kStatusBarY);
}


void Display::UpdateStatusPair(const String& title, int16_t title_width, const String& value, int16_t value_width, int16_t x, int16_t y) {
  SSD1306_->setTextSize(1);
  // title column
  SSD1306_->drawRect(x, y, title_width, kLineHeight, SSD1306_WHITE);
  SSD1306_->setTextColor(SSD1306_WHITE);
  SSD1306_->setCursor(x + kTextOffsetLeft, y + kTextOffsetTop);
  SSD1306_->println(title);
  // value column
  UpdateStatusValue(value, x + title_width, y, value_width);
}


void Display::UpdateStatusValue(const String& value, int16_t x, int16_t y, int16_t width) {
  SSD1306_->setTextSize(1);
  SSD1306_->fillRect(x, y, width, kLineHeight, SSD1306_WHITE);
  SSD1306_->setTextColor(SSD1306_BLACK);
  SSD1306_->setCursor(x + kSpacing, y + kTextOffsetTop);
  SSD1306_->println(value);
}


void Display::DrawContentBorder() {
  SSD1306_->drawRect(0, kLineHeight+kSpacing, kWidth, kHeight-(kLineHeight+kSpacing), SSD1306_WHITE);
}


void Display::ClearContent() {
  SSD1306_->fillRect(kContentSpacing, kContentOffsetTop, kContentWidth, kContentHeight, SSD1306_BLACK);
}

}
