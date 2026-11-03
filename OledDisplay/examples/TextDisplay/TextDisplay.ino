
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

    // Display text
    display.drawString("OLED DISPLAY LIB", 0, 0, false);
    display.drawString("----------------", 0, 10, false);
    display.drawString("6X8 FONT, 21 COLS", 0, 20, false);
    display.drawString("HELLO, WORLD!", 0, 30, false);

    // Display dynamic number
    char line[24];

    snprintf(line, sizeof(line), "MILLIS: %lu", millis());

    display.drawString(line, 0, 44, false);

    // Update OLED
    display.update();

    delay(2000);

    // Hardware scroll pages 0-7 to the left
    display.startScrolling(SCROLL_LEFT, 0, 7, 5);

    delay(5000);

    display.stopScrolling();
}

void loop() {
}
