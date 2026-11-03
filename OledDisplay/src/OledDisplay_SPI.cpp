#include "OledDisplay_SPI.h"
#include "OledDisplay_Init.h"

OledDisplay_SPI::OledDisplay_SPI(int width, int height,
                                 int sck, int mosi,
                                 int cs, int dc,
                                 int reset, int clockSpeed,
                                 int order, int dataMode)
    : OledDisplay(width, height, reset),
      sckPin(sck), mosiPin(mosi), csPin(cs), dcPin(dc),
      spiClockSpeed(clockSpeed), bitOrder(order), spiDataMode(dataMode)
{
}

void OledDisplay_SPI::sendCommand(const uint8_t *commands, int length)
{
    SPI.beginTransaction(SPISettings(spiClockSpeed, (uint8_t)bitOrder, (uint8_t)spiDataMode));
    digitalWrite(dcPin, LOW);
    digitalWrite(csPin, LOW);
    SPI.transfer(const_cast<uint8_t *>(commands), length);
    digitalWrite(csPin, HIGH);
    SPI.endTransaction();
}

void OledDisplay_SPI::sendData(const uint8_t *data, int length)
{
    SPI.beginTransaction(SPISettings(spiClockSpeed, (uint8_t)bitOrder, (uint8_t)spiDataMode));
    digitalWrite(dcPin, HIGH);
    digitalWrite(csPin, LOW);
    SPI.transfer(const_cast<uint8_t *>(data), length);
    digitalWrite(csPin, HIGH);
    SPI.endTransaction();
}

void OledDisplay_SPI::begin()
{
#if defined(ESP32)
    SPI.begin(sckPin, -1, mosiPin, -1);
#else
    SPI.begin();
#endif
    delay(10);

    pinMode(csPin, OUTPUT);
    pinMode(dcPin, OUTPUT);
    digitalWrite(csPin, HIGH);

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
