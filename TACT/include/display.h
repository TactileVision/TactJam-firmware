#ifndef _TACT_DISPLAY_
#define _TACT_DISPLAY_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

namespace tact {

class Display {
  private:
    bool initialized_;
    Adafruit_SSD1306* SSD1306_;
    // display constants
    const uint8_t kWidth = 128;
    const uint8_t kHeight = 64;
    const uint8_t kResetPin = 4;
    const uint8_t kAddress = 0x3C;
    // layout constants
    const int16_t kCharWidth = 6;
    const int16_t kCharHeight = 8;
    const int16_t kTextOffsetLeft = 2;
    const int16_t kTextOffsetTop = 2;
    const int16_t kTextOffsetRight = 1;
    const int16_t kTextOffsetBottom = 1;
    const int16_t kSpacing = 1;
    const int16_t kLineHeight = kCharHeight + kTextOffsetTop + kTextOffsetBottom;
    // boot screen constants
    const int16_t kBootScreenTextX = 20;
    // status bar constants
    const int16_t kStatusBarY = 0;
    const String kModeTitle = "mode";
    const int16_t kModeValueLength = 3;
    const int16_t kModeTitleWidth = kModeTitle.length() * kCharWidth + kTextOffsetLeft + kTextOffsetRight;
    const int16_t kModeValueWidth = kModeValueLength * kCharWidth + kTextOffsetLeft + kTextOffsetRight;
    const int16_t kModeTitleX = 0;
    const int16_t kModeValueX = kModeTitleX + kModeTitleWidth;
    const String kSlotTitle = "slot";
    const int16_t kSlotValueLength = 1;
    const int16_t kSlotTitleWidth = kSlotTitle.length() * kCharWidth + kTextOffsetLeft + kTextOffsetRight;
    const int16_t kSlotValueWidth = kSlotValueLength * kCharWidth + kTextOffsetLeft + kTextOffsetRight;
    const int16_t kSlotTitleX = 52;
    const int16_t kSlotValueX = kSlotTitleX + kSlotTitleWidth;
    const String kAmplitudeTitle = "a";
    const int16_t kAmplitudeValueLength = 4;
    const int16_t kAmplitudeTitleWidth = kAmplitudeTitle.length() * kCharWidth + kTextOffsetLeft + kTextOffsetRight;
    const int16_t kAmplitudeValueWidth = kAmplitudeValueLength * kCharWidth + kTextOffsetLeft + kTextOffsetRight;
    const int16_t kAmplitudeTitleX = 92;
    const int16_t kAmplitudeValueX = kAmplitudeTitleX + kAmplitudeTitleWidth;

    void UpdateStatusBar();
    void UpdateStatusValue(const String& value, int16_t x, int16_t y, int16_t width);
    void UpdateStatusPair(const String& title, int16_t title_width, const String& value, int16_t value_width, int16_t x, int16_t y);


  public:
    Display();
    ~Display();

    bool Initialize();
    void DrawBootScreen();
    void DrawMenuScreen();
    void DrawModeSelection(const String& mode);
    void DrawSlotSelection(uint8_t slot);
    void DrawAmplitude(uint8_t amplitude_perc);

};

}

#endif //_TACTJAM_OLED_
