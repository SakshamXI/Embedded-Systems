
#include <OledDisplay_SPI.h>
#include <math.h>

// -------------------------------------
// Platform-specific OLED configuration
// -------------------------------------

#if defined(ESP32)

OledDisplay_SPI display(128, 64, 18, 23, 14, 27, 16, 100000);

#elif defined(ARDUINO_ARCH_AVR)

OledDisplay_SPI display(128, 64, 13, 11, 10, 7, 8, 100000);

#else
#error "Unsupported board"
#endif

#define CX 64
#define CY 32
#define VERTICES 16

#define D4 2.6
#define D3 3.0
#define FOCAL 100.0

const float TWO_PI_VALUE = 6.28318530f;

// Projected vertex coordinates
int8_t screenX[VERTICES];
int8_t screenY[VERTICES];

// Rotation angles
float aXY = 0;
float aXZ = 0;
float aXW = 0;
float aYZ = 0;
float aYW = 0;
float aZW = 0;

// Precomputed trigonometric values
float cXY, sXY;
float cXZ, sXZ;
float cXW, sXW;
float cYZ, sYZ;
float cYW, sYW;
float cZW, sZW;

unsigned int frame = 0;

// -------------------------------------
// Rotate two coordinates in 4D space
// -------------------------------------

void rotatePair(float &a, float &b, float c, float s) {

    float temp = a * c - b * s;
    b = a * s + b * c;
    a = temp;
}

// -------------------------------------
// Precompute rotation values
// -------------------------------------

void updateRotationValues() {

    cXY = cos(aXY);
    sXY = sin(aXY);

    cXZ = cos(aXZ);
    sXZ = sin(aXZ);

    cXW = cos(aXW);
    sXW = sin(aXW);

    cYZ = cos(aYZ);
    sYZ = sin(aYZ);

    cYW = cos(aYW);
    sYW = sin(aYW);

    cZW = cos(aZW);
    sZW = sin(aZW);
}

// -------------------------------------
// Update all six rotation angles
// -------------------------------------

void updateAngles() {

    aXY += 0.019;
    aXZ += 0.013;
    aXW += 0.021;

    aYZ += 0.011;
    aYW += 0.017;
    aZW += 0.015;

    if (aXY > TWO_PI_VALUE) aXY -= TWO_PI_VALUE;
    if (aXZ > TWO_PI_VALUE) aXZ -= TWO_PI_VALUE;
    if (aXW > TWO_PI_VALUE) aXW -= TWO_PI_VALUE;

    if (aYZ > TWO_PI_VALUE) aYZ -= TWO_PI_VALUE;
    if (aYW > TWO_PI_VALUE) aYW -= TWO_PI_VALUE;
    if (aZW > TWO_PI_VALUE) aZW -= TWO_PI_VALUE;
}

// -------------------------------------
// Generate and project all 16 vertices
// -------------------------------------

void projectVertices() {

    for (byte i = 0; i < VERTICES; i++) {

        float x = (i & 1) ? 1.0 : -1.0;
        float y = (i & 2) ? 1.0 : -1.0;
        float z = (i & 4) ? 1.0 : -1.0;
        float w = (i & 8) ? 1.0 : -1.0;

        // 4D rotations
        rotatePair(x, y, cXY, sXY);
        rotatePair(x, z, cXZ, sXZ);
        rotatePair(x, w, cXW, sXW);

        rotatePair(y, z, cYZ, sYZ);
        rotatePair(y, w, cYW, sYW);

        rotatePair(z, w, cZW, sZW);

        // 4D -> 3D projection
        float divisor4 = D4 - w;

        float x3 = x / divisor4;
        float y3 = y / divisor4;
        float z3 = z / divisor4;

        // 3D -> 2D projection
        float divisor3 = D3 - z3;

        float px = CX + (x3 * FOCAL) / divisor3;
        float py = CY - (y3 * FOCAL) / divisor3;

        int ix = (int)px;
        int iy = (int)py;

        // Screen boundaries
        if (ix < 1) ix = 1;
        if (ix > 126) ix = 126;

        if (iy < 1) iy = 1;
        if (iy > 62) iy = 62;

        screenX[i] = (int8_t)ix;
        screenY[i] = (int8_t)iy;
    }
}

// -------------------------------------
// Draw ordinary cube edges
// -------------------------------------

void drawCubeEdges() {

    for (byte i = 0; i < VERTICES; i++) {

        for (byte bit = 0; bit < 3; bit++) {

            if (!(i & (1 << bit))) {

                byte j = i | (1 << bit);

                display.drawLine(
                    screenX[i],
                    screenY[i],
                    screenX[j],
                    screenY[j],
                    1
                );
            }
        }
    }
}

// -------------------------------------
// Draw 4D W-axis connections
// -------------------------------------

void drawWConnections() {

    for (byte i = 0; i < VERTICES; i++) {

        if (!(i & 8)) {

            byte j = i | 8;

            int x1 = screenX[i];
            int y1 = screenY[i];

            int x2 = screenX[j];
            int y2 = screenY[j];

            display.drawLine(x1, y1, x2, y2, 1);

            int mx = (x1 + x2) / 2;
            int my = (y1 + y2) / 2;

            display.drawPixel(mx, my, 1, false);
        }
    }
}

// -------------------------------------
// Draw vertex markers
// -------------------------------------

void drawVertices() {

    for (byte i = 0; i < VERTICES; i++) {

        int x = screenX[i];
        int y = screenY[i];

        display.drawPixel(x, y, 1, false);

        display.drawPixel(x - 1, y, 1, false);
        display.drawPixel(x + 1, y, 1, false);

        display.drawPixel(x, y - 1, 1, false);
        display.drawPixel(x, y + 1, 1, false);
    }
}

// -------------------------------------
// Draw animated central energy effect
// -------------------------------------

void drawEnergyCore() {

    byte pulse = (frame / 5) % 3;

    display.drawPixel(CX, CY, 1, false);

    if (pulse >= 1) {
        display.drawPixel(CX - 1, CY, 1, false);
        display.drawPixel(CX + 1, CY, 1, false);
    }

    if (pulse >= 2) {
        display.drawPixel(CX, CY - 1, 1, false);
        display.drawPixel(CX, CY + 1, 1, false);
    }
}

// -------------------------------------
// Setup
// -------------------------------------

void setup() {

    display.begin();

    updateRotationValues();
    projectVertices();
}

// -------------------------------------
// Main animation
// -------------------------------------

void loop() {

    display.fillBuffer(0x00);

    updateAngles();
    updateRotationValues();

    projectVertices();

    drawCubeEdges();
    drawWConnections();
    drawVertices();
    drawEnergyCore();

    display.update();

    frame++;

    delay(20);
}
