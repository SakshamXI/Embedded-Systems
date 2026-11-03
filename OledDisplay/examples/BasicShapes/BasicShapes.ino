
#include <OledDisplay_I2C.h>

// -------------------------------------
// Platform-specific I2C configuration
// -------------------------------------

#if defined(ESP32)

OledDisplay_I2C display(128, 64, 21, 22, -1, 0x3C);

#elif defined(ARDUINO_ARCH_AVR)

OledDisplay_I2C display(128, 64, A4, A5, -1, 0x3C);

#else
#error "Unsupported board"
#endif

void setup() {

    display.begin();
    display.clearDisplay();

    // Border
    display.drawRectangle(0, 0, 128, 64, 1, false, false);

    // Lines
    display.drawLine(4, 4, 40, 28, 1);
    display.drawHorizontalLine(4, 40, 32, 1, false);
    display.drawVerticalLine(44, 4, 32, 1, false);

    // Rectangles
    display.drawRectangle(52, 6, 28, 20, 1, false, false);
    display.drawRectangle(84, 6, 28, 20, 1, true, false);

    // Circles
    display.drawCircle(20, 48, 10, 1, false, false);
    display.drawCircle(50, 48, 10, 1, true, false);

    // Triangles
    display.drawTriangle(70, 58, 85, 38, 100, 58, 1, false, false);
    display.drawTriangle(104, 58, 112, 38, 120, 58, 1, true, false);

    // Send framebuffer to OLED
    display.update();
}

void loop() {
}
