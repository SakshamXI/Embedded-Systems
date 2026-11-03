
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

// -------------------------------------
// Scene 1: Lines and rectangles
// -------------------------------------

void scene1() {

    display.fillBuffer(0x00);

    display.drawString("LINES & BOXES", 28, 0, false);

    // Border
    display.drawRectangle(0, 0, 128, 64, 1, false, false);

    // Diagonal lines
    display.drawLine(5, 14, 40, 55, 1);
    display.drawLine(40, 14, 5, 55, 1);

    // Horizontal and vertical lines
    display.drawHorizontalLine(48, 120, 20, 1, false);
    display.drawVerticalLine(84, 25, 58, 1, false);

    // Rectangles
    display.drawRectangle(50, 28, 25, 25, 1, false, false);
    display.drawRectangle(91, 28, 25, 25, 1, true, false);

    display.update();
}

// -------------------------------------
// Scene 2: Circles and triangles
// -------------------------------------

void scene2() {

    display.fillBuffer(0x00);

    display.drawString("CIRCLES & TRIANGLES", 10, 0, false);

    // Circles
    display.drawCircle(20, 25, 12, 1, false, false);
    display.drawCircle(55, 25, 12, 1, true, false);

    // Triangle outlines
    display.drawTriangle(
        80, 38,
        95, 10,
        110, 38,
        1, false, false
    );

    // Filled triangle
    display.drawTriangle(
        15, 60,
        30, 35,
        45, 60,
        1, true, false
    );

    // Additional circles
    display.drawCircle(75, 52, 9, 1, false, false);
    display.drawCircle(110, 52, 9, 1, true, false);

    display.update();
}

// -------------------------------------
// Scene 3: Pixel art and geometry
// -------------------------------------

void scene3() {

    display.fillBuffer(0x00);

    display.drawString("PIXEL GRAPHICS", 25, 0, false);

    // Pixel grid
    for (int x = 5; x < 45; x += 5) {
        for (int y = 15; y < 55; y += 5) {
            display.drawPixel(x, y, 1, false);
        }
    }

    // Concentric circles
    display.drawCircle(85, 34, 25, 1, false, false);
    display.drawCircle(85, 34, 18, 1, false, false);
    display.drawCircle(85, 34, 10, 1, false, false);

    // Center point
    display.drawPixel(85, 34, 1, false);

    // Cross lines
    display.drawLine(55, 34, 115, 34, 1);
    display.drawLine(85, 5, 85, 60, 1);

    display.update();
}

// -------------------------------------
// Setup
// -------------------------------------

void setup() {

    display.begin();
    display.clearDisplay();
}

// -------------------------------------
// Main loop
// -------------------------------------

void loop() {

    scene1();
    delay(2500);

    scene2();
    delay(2500);

    scene3();
    delay(2500);
}
