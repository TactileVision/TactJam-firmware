#ifndef _TACT_DISPLAY_
#define _TACT_DISPLAY_

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>

namespace tact {

class Display {
  private:
    Adafruit_SSD1306* SSD1306_;
    uint8_t width_;
    uint8_t height_;
    uint8_t reset_pin_;
    uint8_t address_;
    bool initialized_;

    const int16_t kCharWidth = 6;
    const int16_t kCharHeight = 8;
    const int16_t kTextOffsetLeft = 2;
    const int16_t kTextOffsetTop = 2;
    const int16_t kTextOffsetRight = 1;
    const int16_t kTextOffsetBottom = 1;
    const int16_t kSpacing = 1;
    const int16_t kLineHeight = kCharHeight + kTextOffsetTop + kTextOffsetBottom;

    void DrawStatusBar();
    void DrawMenuValue(const String& value, int16_t x, int16_t y);
    void DrawMenuPair(const String& title, const String& value, int16_t x, int16_t y);


  public:
    Display();
    ~Display();

    bool Initialize();
    void DrawBootScreen();
    void DrawMenuScreen();
    void UpdateModeSelection(const String& mode);
    void UpdateSlotSelection(uint8_t slot);
    void UpdateAmplitude(uint8_t amplitude_perc);

};

}

#endif //_TACTJAM_OLED_
