#ifndef OLED_DISPLAY_I2C_H
#define OLED_DISPLAY_I2C_H

#include <Wire.h>
#include "OledDisplay.h"

class OledDisplay_I2C : public OledDisplay
{
public:
    int sda, scl, address;

    OledDisplay_I2C(int width = 128, int height = 64,
                    int sda = 21, int scl = 22,
                    int reset = 16,
                    int address = 0x3C);

    void begin() override;
    void sendCommand(const uint8_t *commands, int length) override;
    void sendData(const uint8_t *data, int length) override;

private:
    void sendChunks(uint8_t control, const uint8_t *data, int length);
};

#endif
