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
    // content screen constants
    const int16_t kContentSpacing = 4;
    const int16_t kContentOffsetTop = kLineHeight+kSpacing+kContentSpacing;
    const int16_t kContentWidth = 120;
    const int16_t kContentHeight = 44;
    const int16_t kContentCenterX = 64;
    const int16_t kContentCenterY = (kContentHeight/2)+kContentOffsetTop;

    /**
     * @brief Update the status bar on top of the screen.
     *        Update methods do only update the setup but do not display anything.
     * 
     * @param mode the new mode selected by the mode encoder.
     * @param slot the new slot selected by the slot encoder.
     * @param amplitude the new amplitude selected by the amplitude encoder.
     */
    void UpdateStatusBar(const String& mode, const uint8_t slot, const uint8_t amplitude);

    /**
     * @brief Update just the value of a key-value-pair.
     *        Update methods do only update the setup but do not display anything.
     * 
     * @param value the new text to display.
     * @param x the horizontal position of the text (left hand side).
     * @param y the vertical postion of the text (top).
     * @param width the width of the text (in pixels).
     */
    void UpdateStatusValue(const String& value, int16_t x, int16_t y, int16_t width);

    /**
     * @brief Update key (title) and value of a menu item.
     *        Update methods do only update the setup but do not display anything.
     * 
     * @param title the text of the key.
     * @param title_width the width of the title text (in pixels).
     * @param value the text of the value.
     * @param value_width the width of the value text (in pixels).
     * @param x the horizontal position of the text (left hand side).
     * @param y the vertical postion of the text (top).
     */
    void UpdateStatusPair(const String& title, int16_t title_width, const String& value, int16_t value_width, int16_t x, int16_t y);

    /**
     * @brief Draw the rectangle around the content section (below status bar).
     * 
     */
    void DrawContentBorder();

    /**
     * @brief Clear the entire content inside the content section.
     * 
     */
    void ClearContent();

  public:
    Display();
    ~Display();

    /**
     * @brief Initialize the display driver.
     * 
     * @return true initialization succeeded.
     * @return false initialization of the display driver (via I2C) failed.
     */
    bool Initialize();

    /**
     * @brief Draw the screen that is shown while initializing the system.
     * 
     */
    void DrawBootScreen();

    /**
     * @brief Draw the entire screen (status bar and content section).
     * 
     * @param mode the new mode selected by the mode encoder.
     * @param slot the new slot selected by the slot encoder.
     * @param amplitude the new amplitude selected by the amplitude encoder.
     */
    void DrawMenuScreen(const String& mode, const uint8_t slot, const uint8_t amplitude);

    /**
     * @brief Update and display the selected mode.
     * 
     * @param mode the new mode selected by the mode encoder.
     */
    void DrawModeSelection(const String& mode);

    /**
     * @brief Update and display the selected slot.
     * 
     * @param slot the new slot selected by the slot encoder.
     */
    void DrawSlotSelection(uint8_t slot);

    /**
     * @brief Update and display the selected amplitude.
     * 
     * @param amplitude_perc the amplitude in percent (0-100).
     */
    void DrawAmplitude(uint8_t amplitude_perc);

    /**
     * @brief Display a large teaser message in the center of the content section.
     * 
     * @param text the text to display.
     */
    void DrawContentTeaser(const String& text);

    // TODO: This method declaration may change depending on the information we need to display.
    /**
     * @brief Display detailed information of a given tacton.
     * 
     * @param slot the slot where the tacton is stored on the device.
     * @param uuid the device's UUID.
     * @param instruction_size the number of vtp instructions.
     * @param length_millis the length in milliseconds.
     */
    void DrawTactonDetails(const uint8_t slot, const String& uuid, uint32_t instruction_size, uint64_t length_millis);

};

}

#endif //_TACTJAM_OLED_
