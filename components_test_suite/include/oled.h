#ifndef _TACTJAM_OLED_
#define _TACTJAM_OLED_

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

namespace tactjam {
namespace display {

class OLED_i2c {
  private:
    Adafruit_SSD1306* SSD1306_;
    uint8_t width_;
    uint8_t height_;
    uint8_t reset_pin_;
    uint8_t address_;
    bool initialized_;

  public:
    OLED_i2c() {
      width_ = 128;
      height_ = 64;
      reset_pin_ = 4;
      address_ = 0x3C;
      initialized_ = false;
      SSD1306_ = new Adafruit_SSD1306(width_, height_, &Wire, reset_pin_);
    }

    ~OLED_i2c() {
      SSD1306_->~Adafruit_SSD1306();
      SSD1306_ = nullptr;
    }

    bool Initialize() {
      initialized_ = SSD1306_->begin(SSD1306_SWITCHCAPVCC, address_);
      return initialized_;
    }

    void TestStaticScreen() {
      // lazy evaltuation should prevent call to Initialize if already initialized
      if (!initialized_ && !Initialize()) {
        return;
      }
      SSD1306_->clearDisplay();
      SSD1306_->invertDisplay(false);
      SSD1306_->setTextColor(SSD1306_WHITE);
      SSD1306_->setTextSize(1);
      SSD1306_->setCursor(0,0);
      SSD1306_->println("TactJam v0.0.1");
      SSD1306_->setCursor(0,10);
      SSD1306_->println("test screen");
      SSD1306_->display();
    }

    void TestDrawLines() {
      // lazy evaltuation should prevent call to Initialize if already initialized
      if (!initialized_ && !Initialize()) {
        return;
      }
      int16_t i;
      static const int16_t step = 4;
      SSD1306_->clearDisplay();
      for(i=0; i<width_; i+=step) {
        SSD1306_->drawLine(0, 0, i, height_-1, SSD1306_WHITE);
        SSD1306_->display();
        delay(1);
      }
      for(i=height_; i>=0; i-=step) {
        SSD1306_->drawLine(0, 0, width_-1, i, SSD1306_WHITE);
        SSD1306_->display();
        delay(1);
      }
      delay(250);
      SSD1306_->clearDisplay();
      SSD1306_->display();
    }

    void TestMenuScreen() {
      // lazy evaltuation should prevent call to Initialize if already initialized
      if (!initialized_ && !Initialize()) {
        return;
      }
      SSD1306_->clearDisplay();
      SSD1306_->invertDisplay(false);
      DrawStatusBar();
      SSD1306_->drawRect(0, 12, 128, 64-12, SSD1306_WHITE);
      SSD1306_->setTextColor(SSD1306_WHITE);
      SSD1306_->setTextSize(2);
      SSD1306_->setCursor(20, 24);
      SSD1306_->println("TactJam");
      SSD1306_->setTextSize(1);
      SSD1306_->setCursor(16, 42);
      SSD1306_->println(" v0.1");
      SSD1306_->display();
    }

  private:
    const int16_t kCharWidth = 6;
    const int16_t kCharHeight = 8;
    const int16_t kTextOffsetLeft = 2;
    const int16_t kTextOffsetTop = 2;
    const int16_t kTextOffsetRight = 1;
    const int16_t kTextOffsetBottom = 1;
    const int16_t kSpacing = 1;
    const int16_t kLineHeight = kCharHeight + kTextOffsetTop + kTextOffsetBottom;

    void DrawStatusBar() {
      DrawMenuPair("mode", "jam", 0, 0);
      DrawMenuPair("slot", "1", 52, 0);
      DrawMenuPair("L", "100%", 92, 0);
    }

    void DrawMenuPair(const String& title, const String& value, int16_t x, int16_t y) {
      SSD1306_->setTextSize(1);
      // title column
      auto title_width = title.length() * kCharWidth;
      auto title_line_width = title_width + kTextOffsetLeft + kTextOffsetRight;
      SSD1306_->drawRect(x, y, title_line_width, kLineHeight, SSD1306_WHITE);
      SSD1306_->setTextColor(SSD1306_WHITE);
      SSD1306_->setCursor(x + kTextOffsetLeft, y + kTextOffsetTop);
      SSD1306_->println(title);
      // value column
      auto value_width = value.length() * kCharWidth;
      auto value_line_width = value_width + kTextOffsetLeft + kTextOffsetRight;
      SSD1306_->fillRect(x + title_line_width, y, value_line_width, kLineHeight, SSD1306_WHITE);
      SSD1306_->setTextColor(SSD1306_BLACK);
      SSD1306_->setCursor(x + title_line_width + kSpacing, y + kTextOffsetTop);
      SSD1306_->println(value);
    }
};

}
}

#endif //_TACTJAM_OLED_
