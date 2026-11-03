#ifndef OLED_DISPLAY_SPI_H
#define OLED_DISPLAY_SPI_H

#include <SPI.h>
#include "OledDisplay.h"

class OledDisplay_SPI : public OledDisplay
{
public:
    int sckPin, mosiPin, csPin, dcPin;
    int spiClockSpeed, bitOrder, spiDataMode;

    OledDisplay_SPI(int width = 128, int height = 64,
                    int sck = 18, int mosi = 23,
                    int cs = 14, int dc = 27,
                    int reset = 16, int clockSpeed = 1000000,
                    int order = MSBFIRST,
                    int dataMode = SPI_MODE0);

    void begin() override;
    void sendCommand(const uint8_t *commands, int length) override;
    void sendData(const uint8_t *data, int length) override;
};

#endif
