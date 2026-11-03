#include "OledDisplay_I2C.h"
#include "OledDisplay_Init.h"

OledDisplay_I2C::OledDisplay_I2C(int width, int height,
                                 int sdaPin, int sclPin,
                                 int reset, int addr)
    : OledDisplay(width, height, reset),
      sda(sdaPin), scl(sclPin), address(addr)
{
}

// Wire buffers are typically 32 bytes: 1 control byte + up to 31 payload bytes.
void OledDisplay_I2C::sendChunks(uint8_t control, const uint8_t *data, int length)
{
    int current = 0;
    while (current < length)
    {
        int chunk = min(31, length - current);

        Wire.beginTransmission(address);
        Wire.write(control);
        Wire.write(data + current, chunk);
        Wire.endTransmission();

        current += chunk;
    }
}

void OledDisplay_I2C::sendCommand(const uint8_t *commands, int length)
{
    sendChunks(0x00, commands, length);
}

void OledDisplay_I2C::sendData(const uint8_t *data, int length)
{
    sendChunks(0x40, data, length);
}

void OledDisplay_I2C::begin()
{
#if defined(ESP32)
    Wire.begin(sda, scl);
#else
    Wire.begin();
#endif
    delay(10);

    if (resetPin >= 0)
    {
        pinMode(resetPin, OUTPUT);
        digitalWrite(resetPin, LOW);
        delay(10);
        digitalWrite(resetPin, HIGH);
        delay(10);
    }

    uint8_t buffer[OLED_INIT_SIZE];
    memcpy_P(buffer, oledInitSequence, OLED_INIT_SIZE);
    sendCommand(buffer, OLED_INIT_SIZE);
}
