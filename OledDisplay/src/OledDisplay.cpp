#include "OledDisplay.h"
#include "fonts/Font6x8.h"

static void swapValues(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

OledDisplay::OledDisplay(int w, int h, int rp)
{
    width = w;
    height = h;
    resetPin = rp;

    frameBuffer = new uint8_t[width * height / 8];
    memset(frameBuffer, 0, width * height / 8);
}

OledDisplay::~OledDisplay()
{
    delete[] frameBuffer;
}

// ================= SCREEN =================

void OledDisplay::setAddressWindow(uint8_t page, uint8_t column)
{
    uint8_t cmds[] = {0x21, column, (uint8_t)(width - 1), 0x22, page, (uint8_t)(height / 8 - 1)};
    sendCommand(cmds, sizeof(cmds));
}

void OledDisplay::update()
{
    setAddressWindow(0, 0);
    sendData(frameBuffer, (width * height) / 8);
}

void OledDisplay::clearDisplay()
{
    memset(frameBuffer, 0, (width * height) / 8);
    update();
}

void OledDisplay::setPower(bool on)
{
    uint8_t cmd = on ? 0xAF : 0xAE;
    sendCommand(&cmd, 1);
}

void OledDisplay::setContrast(uint8_t level)
{
    uint8_t cmds[] = {0x81, level};
    sendCommand(cmds, 2);
}

// ================= PIXEL =================

void OledDisplay::drawPixel(int x, int y, int color, bool immediateUpdate)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    int page = y / 8;
    int bit = y % 8;
    int index = page * width + x;

    if (color)
        frameBuffer[index] |= (1 << bit);
    else
        frameBuffer[index] &= ~(1 << bit);

    if (immediateUpdate)
    {
        setAddressWindow(page, x);
        sendData(&frameBuffer[index], 1);
    }
}

// ================= LINES =================

void OledDisplay::drawHorizontalLine(int x1, int x2, int y, int color, bool immediateUpdate)
{
    if (x1 > x2)
        swapValues(&x1, &x2);
    if (x1 < 0) x1 = 0;
    if (x2 > width - 1) x2 = width - 1;
    for (int i = x1; i <= x2; i++)
        drawPixel(i, y, color, false);
    if (immediateUpdate)
        update();
}

void OledDisplay::drawVerticalLine(int x, int y1, int y2, int color, bool immediateUpdate)
{
    if (y1 > y2)
        swapValues(&y1, &y2);
    if (y1 < 0) y1 = 0;
    if (y2 > height - 1) y2 = height - 1;
    for (int i = y1; i <= y2; i++)
        drawPixel(x, i, color, false);
    if (immediateUpdate)
        update();
}

void OledDisplay::drawLine(int x0, int y0, int x1, int y1, int color)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (true)
    {
        drawPixel(x0, y0, color, false);
        if (x0 == x1 && y0 == y1)
            break;
        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

// ================= SHAPES =================

void OledDisplay::drawRectangle(int x, int y, int w, int h, int color, bool filled, bool immediateUpdate)
{
    if (!filled)
    {
        drawHorizontalLine(x, x + w - 1, y, color, false);
        drawHorizontalLine(x, x + w - 1, y + h - 1, color, false);
        drawVerticalLine(x, y, y + h - 1, color, false);
        drawVerticalLine(x + w - 1, y, y + h - 1, color, false);
    }
    else
    {
        for (int i = y; i < y + h; i++)
            drawHorizontalLine(x, x + w - 1, i, color, false);
    }
    if (immediateUpdate)
        update();
}

void OledDisplay::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int color, bool filled, bool immediateUpdate)
{
    if (filled)
    {
        if (y1 > y2) { swapValues(&y1, &y2); swapValues(&x1, &x2); }
        if (y1 > y3) { swapValues(&y1, &y3); swapValues(&x1, &x3); }
        if (y2 > y3) { swapValues(&y2, &y3); swapValues(&x2, &x3); }

        float slope12 = (y2 != y1) ? (float)(x2 - x1) / (y2 - y1) : 0;
        float slope13 = (y3 != y1) ? (float)(x3 - x1) / (y3 - y1) : 0;
        float slope23 = (y3 != y2) ? (float)(x3 - x2) / (y3 - y2) : 0;

        float xLeft = x1, xRight = x1;

        for (int y = y1; y < y2; y++)
        {
            drawHorizontalLine((int)xLeft, (int)xRight, y, color, false);
            xLeft += slope12;
            xRight += slope13;
        }

        xLeft = x2;
        for (int y = y2; y <= y3; y++)
        {
            drawHorizontalLine((int)xLeft, (int)xRight, y, color, false);
            xLeft += slope23;
            xRight += slope13;
        }
    }
    else
    {
        drawLine(x1, y1, x2, y2, color);
        drawLine(x2, y2, x3, y3, color);
        drawLine(x3, y3, x1, y1, color);
    }

    if (immediateUpdate)
        update();
}

void OledDisplay::drawCircle(int xc, int yc, int r, int color, bool filled, bool immediateUpdate)
{
    int x = 0;
    int y = r;
    int p = 1 - r;

    while (x <= y)
    {
        if (filled)
        {
            drawHorizontalLine(xc - x, xc + x, yc + y, color, false);
            drawHorizontalLine(xc - x, xc + x, yc - y, color, false);
            drawHorizontalLine(xc - y, xc + y, yc + x, color, false);
            drawHorizontalLine(xc - y, xc + y, yc - x, color, false);
        }
        else
        {
            drawPixel(xc + x, yc + y, color, false);
            drawPixel(xc - x, yc + y, color, false);
            drawPixel(xc + x, yc - y, color, false);
            drawPixel(xc - x, yc - y, color, false);
            drawPixel(xc + y, yc + x, color, false);
            drawPixel(xc - y, yc + x, color, false);
            drawPixel(xc + y, yc - x, color, false);
            drawPixel(xc - y, yc - x, color, false);
        }

        x++;

        if (p < 0)
        {
            p += 2 * x + 1;
        }
        else
        {
            y--;
            p += 2 * (x - y) + 1;
        }
    }

    if (immediateUpdate)
        update();
}

// ================= BUFFER EFFECTS =================

void OledDisplay::fillBuffer(uint8_t pattern)
{
    memset(frameBuffer, pattern, (width * height) / 8);
}

void OledDisplay::invertBuffer(bool immediateUpdate)
{
    for (int i = 0; i < (width * height) / 8; i++)
        frameBuffer[i] = ~frameBuffer[i];
    if (immediateUpdate)
        update();
}

void OledDisplay::invertDisplayHardware(bool enable)
{
    uint8_t cmd = enable ? 0xA7 : 0xA6;
    sendCommand(&cmd, 1);
}

// ================= SCROLL =================

void OledDisplay::startScrolling(bool direction, uint8_t startPage, uint8_t endPage, uint8_t speed)
{
    const uint16_t frameLookup[] = {5, 64, 128, 256, 3, 4, 25, 2};
    uint8_t frameVal = 9;

    for (uint8_t i = 0; i < 8; i++)
        if (frameLookup[i] == speed)
            frameVal = i;

    if (frameVal == 9)
        return;

    uint8_t cmds[] = {
        (uint8_t)(direction ? 0x26 : 0x27),
        0x00, startPage, frameVal, endPage, 0x00, 0xFF};
    sendCommand(cmds, 7);

    uint8_t startCmd = 0x2F;
    sendCommand(&startCmd, 1);
}

void OledDisplay::stopScrolling()
{
    uint8_t cmd = 0x2E;
    sendCommand(&cmd, 1);
}

// ================= BITMAP & TEXT =================

void OledDisplay::drawBitmap(int x, int y, const uint8_t *bitmap, int w, int h, bool immediateUpdate)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int index = (i / 8) * w + j;
            uint8_t bit = (bitmap[index] >> (i % 8)) & 1;
            drawPixel(x + j, y + i, bit, false);
        }
    }
    if (immediateUpdate)
        update();
}

void OledDisplay::drawChar(char c, int x, int y, bool immediateUpdate)
{
    if (c >= 'a' && c <= 'z')
        c -= 32; // font has no lowercase glyphs
    if (c < FONT6X8_FIRST_CHAR || c > FONT6X8_LAST_CHAR)
        return;

    int fontIndex = FONT6X8_HEADER + (c - FONT6X8_FIRST_CHAR) * FONT6X8_WIDTH;
    uint8_t buffer[FONT6X8_WIDTH];

    memcpy_P(buffer, &SSD1306_FONT_6x8[fontIndex], FONT6X8_WIDTH);
    drawBitmap(x, y, buffer, FONT6X8_WIDTH, 8, immediateUpdate);
}

void OledDisplay::drawString(const char *text, int x, int y, bool immediateUpdate)
{
    for (int i = 0; text[i] != '\0'; i++)
        drawChar(text[i], x + FONT6X8_WIDTH * i, y, false);
    if (immediateUpdate)
        update();
}
