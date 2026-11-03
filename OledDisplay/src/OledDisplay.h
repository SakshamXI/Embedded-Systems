#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>

#define SCROLL_RIGHT true
#define SCROLL_LEFT false

// Base class: framebuffer + drawing API for SSD1306 128x64 OLEDs.
// Transport (SPI / I2C) is implemented by subclasses.
class OledDisplay
{
public:
    int width, height, resetPin;
    uint8_t *frameBuffer;

    OledDisplay(int w, int h, int rp);
    virtual ~OledDisplay();

    // --- Transport (implemented by subclasses) ---
    virtual void begin() = 0;
    virtual void sendCommand(const uint8_t *commands, int length) = 0;
    virtual void sendData(const uint8_t *data, int length) = 0;

    // --- Screen ---
    void setAddressWindow(uint8_t page, uint8_t column);
    void update();
    void clearDisplay();
    void setPower(bool on);
    void setContrast(uint8_t level);

    // --- Pixel ---
    void drawPixel(int x, int y, int color, bool immediateUpdate = false);

    // --- Lines ---
    void drawHorizontalLine(int x1, int x2, int y, int color, bool immediateUpdate = false);
    void drawVerticalLine(int x, int y1, int y2, int color, bool immediateUpdate = false);
    void drawLine(int x0, int y0, int x1, int y1, int color);

    // --- Shapes ---
    void drawRectangle(int x, int y, int w, int h, int color, bool filled = false, bool immediateUpdate = false);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, bool filled = false, bool immediateUpdate = false);
    void drawCircle(int xc, int yc, int r, int color, bool filled = false, bool immediateUpdate = false);

    // --- Buffer effects ---
    void fillBuffer(uint8_t pattern);
    void invertBuffer(bool immediateUpdate = false);
    void invertDisplayHardware(bool enable);

    // --- Hardware scroll (speed = frames between steps: 2,3,4,5,25,64,128,256) ---
    void startScrolling(bool direction, uint8_t startPage, uint8_t endPage, uint8_t speed);
    void stopScrolling();

    // --- Bitmap & text ---
    // Bitmap format: page-major, 1 byte = 8 vertical pixels (bit0 = top).
    void drawBitmap(int x, int y, const uint8_t *bitmap, int w, int h, bool immediateUpdate = false);
    void drawChar(char c, int x, int y, bool immediateUpdate = false);
    void drawString(const char *text, int x, int y, bool immediateUpdate = false);

private:
    OledDisplay(const OledDisplay &) = delete;
    OledDisplay &operator=(const OledDisplay &) = delete;
};

#endif
