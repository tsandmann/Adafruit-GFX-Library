/*!
 * @file Adafruit_SPITFT.cpp
 *
 * @mainpage Adafruit SPI TFT Displays (and some others)
 *
 * @section intro_sec Introduction
 *
 * Part of Adafruit's GFX graphics library. Originally this class was
 * written to handle a range of color TFT displays connected via SPI,
 * but over time this library and some display-specific subclasses have
 * mutated to include some color OLEDs as well as parallel-interfaced
 * displays. The name's been kept for the sake of older code.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!

 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit_GFX">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Author
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries,
 * with contributions from the open source community.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 * Adapted for use with the c't-Bot teensy framework and ported to C++14 by Timo Sandmann
 */

#include "Adafruit_SPITFT.h"
#include "Arduino.h"


Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t rst) : Adafruit_SPITFT(w, h, &SPI, cs, dc, rst) {}

Adafruit_SPITFT::Adafruit_SPITFT(uint16_t w, uint16_t h, SPIClass* spiClass, int8_t cs, int8_t dc, int8_t rst)
    : Adafruit_GFX(w, h), _spi { spiClass }, _rst { rst }, _cs { cs }, _dc { dc } {}

void Adafruit_SPITFT::initSPI(uint32_t freq) {
    ::pinMode(_cs, OUTPUT);
    ::digitalWriteFast(_cs, 1); // Deselect

    ::pinMode(_dc, OUTPUT);
    ::digitalWriteFast(_dc, 1); // Data mode

    _spi_settings = SPISettings(freq, MSBFIRST, SPI_MODE0);
    _spi->begin();

    if (_rst >= 0) {
        // Toggle _rst low to reset
        ::pinMode(_rst, OUTPUT);
        ::digitalWriteFast(_rst, 1);
        ::delay(100);
        ::digitalWriteFast(_rst, 0);
        ::delay(100);
        ::digitalWriteFast(_rst, 1);
        ::delay(200);
    }
}

void Adafruit_SPITFT::writePixel(int16_t x, int16_t y, uint16_t color) {
    if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
        setAddrWindow(x, y, 1, 1);
        SPI_WRITE16(color);
    }
}

void Adafruit_SPITFT::writePixels(uint16_t* colors, uint32_t len, bool, bool) const {
    while (len--) {
        SPI_WRITE16(*colors++);
    }
}

void Adafruit_SPITFT::writeColor(uint16_t color, uint32_t len) {
    switch (len) {
        case 0: return;

        case 4: SPI_WRITE16(color); [[fallthrough]];

        case 3: SPI_WRITE16(color); [[fallthrough]];

        case 2: SPI_WRITE16(color); [[fallthrough]];

        case 1: SPI_WRITE16(color); break;

        default: {
            const uint16_t c { static_cast<uint16_t>(((color & 0xff) << 8) | (color >> 8)) };
            const uint32_t count { std::min(SPI_BLOCKSIZE, len) };
            for (uint32_t i { 0 }; i < count; ++i) {
                _spi_buffer[i] = c;
            }

            const uint32_t n { len / SPI_BLOCKSIZE };
            uint32_t i;
            for (i = 0; i < n; ++i) {
                _spi->transfer(_spi_buffer, nullptr, sizeof(_spi_buffer));
            }
            _spi->transfer(_spi_buffer, nullptr, (len - i * SPI_BLOCKSIZE) * sizeof(uint16_t));
            break;
        }
    }
}

void Adafruit_SPITFT::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (w && h) { // Nonzero width and height?
        if (w < 0) { // If negative width...
            x += w + 1; //   Move X to left edge
            w = -w; //   Use positive width
        }
        if (x < _width) { // Not off right
            if (h < 0) { // If negative height...
                y += h + 1; //   Move Y to top edge
                h = -h; //   Use positive height
            }
            if (y < _height) { // Not off bottom
                const int32_t x2 { x + w - 1 };
                if (x2 >= 0) { // Not off left
                    const int32_t y2 { y + h - 1 };
                    if (y2 >= 0) { // Not off top
                        // Rectangle partly or fully overlaps screen
                        if (x < 0) {
                            x = 0;
                            w = x2 + 1;
                        } // Clip left
                        if (y < 0) {
                            y = 0;
                            h = y2 + 1;
                        } // Clip top
                        if (x2 >= _width) {
                            w = _width - x;
                        } // Clip right
                        if (y2 >= _height) {
                            h = _height - y;
                        } // Clip bottom
                        writeFillRectPreclipped(x, y, w, h, color);
                    }
                }
            }
        }
    }
}

void Adafruit_SPITFT::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if ((y >= 0) && (y < _height) && w) { // Y on screen, nonzero width
        if (w < 0) { // If negative width...
            x += w + 1; //   Move X to left edge
            w = -w; //   Use positive width
        }
        if (x < _width) { // Not off right
            const int32_t x2 { x + w - 1 };
            if (x2 >= 0) { // Not off left
                // Line partly or fully overlaps screen
                if (x < 0) {
                    x = 0;
                    w = x2 + 1;
                } // Clip left
                if (x2 >= _width) {
                    w = _width - x;
                } // Clip right
                writeFillRectPreclipped(x, y, w, 1, color);
            }
        }
    }
}

void Adafruit_SPITFT::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if ((x >= 0) && (x < _width) && h) { // X on screen, nonzero height
        if (h < 0) { // If negative height...
            y += h + 1; //   Move Y to top edge
            h = -h; //   Use positive height
        }
        if (y < _height) { // Not off bottom
            const int32_t y2 { y + h - 1 };
            if (y2 >= 0) { // Not off top
                // Line partly or fully overlaps screen
                if (y < 0) {
                    y = 0;
                    h = y2 + 1;
                } // Clip top
                if (y2 >= _height) {
                    h = _height - y;
                } // Clip bottom
                writeFillRectPreclipped(x, y, 1, h, color);
            }
        }
    }
}

void Adafruit_SPITFT::writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    setAddrWindow(x, y, w, h);
    writeColor(color, (uint32_t) w * h);
}

void Adafruit_SPITFT::drawPixel(int16_t x, int16_t y, uint16_t color) {
    // Clip first...
    if ((x >= 0) && (x < _width) && (y >= 0) && (y < _height)) {
        // THEN set up transaction (if needed) and draw...
        startWrite();
        setAddrWindow(x, y, 1, 1);
        SPI_WRITE16(color);
        endWrite();
    }
}

void Adafruit_SPITFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (w && h) { // Nonzero width and height?
        if (w < 0) { // If negative width...
            x += w + 1; //   Move X to left edge
            w = -w; //   Use positive width
        }
        if (x < _width) { // Not off right
            if (h < 0) { // If negative height...
                y += h + 1; //   Move Y to top edge
                h = -h; //   Use positive height
            }
            if (y < _height) { // Not off bottom
                const int32_t x2 { x + w - 1 };
                if (x2 >= 0) { // Not off left
                    const int32_t y2 { y + h - 1 };
                    if (y2 >= 0) { // Not off top
                        // Rectangle partly or fully overlaps screen
                        if (x < 0) {
                            x = 0;
                            w = x2 + 1;
                        } // Clip left
                        if (y < 0) {
                            y = 0;
                            h = y2 + 1;
                        } // Clip top
                        if (x2 >= _width) {
                            w = _width - x;
                        } // Clip right
                        if (y2 >= _height) {
                            h = _height - y;
                        } // Clip bottom
                        startWrite();
                        writeFillRectPreclipped(x, y, w, h, color);
                        endWrite();
                    }
                }
            }
        }
    }
}

void Adafruit_SPITFT::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if ((y >= 0) && (y < _height) && w) { // Y on screen, nonzero width
        if (w < 0) { // If negative width...
            x += w + 1; //   Move X to left edge
            w = -w; //   Use positive width
        }
        if (x < _width) { // Not off right
            const int32_t x2 { x + w - 1 };
            if (x2 >= 0) { // Not off left
                // Line partly or fully overlaps screen
                if (x < 0) {
                    x = 0;
                    w = x2 + 1;
                } // Clip left
                if (x2 >= _width) {
                    w = _width - x;
                } // Clip right
                startWrite();
                writeFillRectPreclipped(x, y, w, 1, color);
                endWrite();
            }
        }
    }
}

void Adafruit_SPITFT::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    if ((x >= 0) && (x < _width) && h) { // X on screen, nonzero height
        if (h < 0) { // If negative height...
            y += h + 1; //   Move Y to top edge
            h = -h; //   Use positive height
        }
        if (y < _height) { // Not off bottom
            const int32_t y2 { y + h - 1 };
            if (y2 >= 0) { // Not off top
                // Line partly or fully overlaps screen
                if (y < 0) {
                    y = 0;
                    h = y2 + 1;
                } // Clip top
                if (y2 >= _height) {
                    h = _height - y;
                } // Clip bottom
                startWrite();
                writeFillRectPreclipped(x, y, 1, h, color);
                endWrite();
            }
        }
    }
}

void Adafruit_SPITFT::pushColor(uint16_t color) {
    startWrite();
    SPI_WRITE16(color);
    endWrite();
}

void Adafruit_SPITFT::drawRGBBitmap(int16_t x, int16_t y, uint16_t* pcolors, int16_t w, int16_t h) {
    int16_t x2, y2; // Lower-right coord
    if ((x >= _width) || // Off-edge right
        (y >= _height) || // " top
        ((x2 = (x + w - 1)) < 0) || // " left
        ((y2 = (y + h - 1)) < 0)) {
        return; // " bottom
    }

    int16_t bx1 { 0 }, by1 { 0 }; // Clipped top-left within bitmap
    const int16_t saveW { w }; // Save original bitmap width value
    if (x < 0) { // Clip left
        w += x;
        bx1 = -x;
        x = 0;
    }
    if (y < 0) { // Clip top
        h += y;
        by1 = -y;
        y = 0;
    }
    if (x2 >= _width) {
        w = _width - x; // Clip right
    }
    if (y2 >= _height) {
        h = _height - y; // Clip bottom
    }

    pcolors += by1 * saveW + bx1; // Offset bitmap ptr to clipped top-left
    startWrite();
    setAddrWindow(x, y, w, h); // Clipped area
    while (h--) { // For each (clipped) scanline...
        writePixels(pcolors, w); // Push one (clipped) row
        pcolors += saveW; // Advance pointer by one full (unclipped) line
    }
    endWrite();
}
