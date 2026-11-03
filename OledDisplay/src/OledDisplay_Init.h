#ifndef OLED_DISPLAY_INIT_H
#define OLED_DISPLAY_INIT_H

#include <Arduino.h>

// SSD1306 128x64 power-up command sequence (shared by SPI and I2C).
static const uint8_t oledInitSequence[] PROGMEM = {
    0xAE,        // display off
    0xD5, 0x80,  // clock divide / oscillator
    0xA8, 0x3F,  // multiplex ratio (64 rows)
    0xD3, 0x00,  // display offset
    0x40,        // start line 0
    0x8D, 0x14,  // charge pump on
    0xD9, 0x22,  // pre-charge
    0xDB, 0x30,  // VCOMH deselect
    0x20, 0x00,  // horizontal addressing mode
    0xA0,        // segment remap
    0xC0,        // COM scan direction
    0xDA, 0x12,  // COM pins config
    0x81, 0xFF,  // contrast
    0xA4,        // resume from RAM
    0xA6,        // normal (non-inverted)
    0xAF         // display on
};

#define OLED_INIT_SIZE ((int)sizeof(oledInitSequence))

#endif
