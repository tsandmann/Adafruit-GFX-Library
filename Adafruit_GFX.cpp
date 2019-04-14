/*
 * This is the core graphics library for all our displays, providing a common
 * set of graphics primitives (points, lines, circles, etc.).  It needs to be
 * paired with a hardware-specific library for each display device we carry
 * (to handle the lower-level functions).
 *
 * Adafruit invests time and resources providing this open source code, please
 * support Adafruit & open-source hardware by purchasing products from Adafruit!
 *
 * Copyright (c) 2013 Adafruit Industries.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Adapted for use with the c't-Bot teensy framework and ported to C++14 by Timo Sandmann
 */

#include "Adafruit_GFX.h"
#include "glcdfont.h"
#undef abs

#include <algorithm>
#include <cmath>
#include <cstring>


Adafruit_GFX::Adafruit_GFX(int16_t w, int16_t h)
    : WIDTH(w), HEIGHT(h), _width { WIDTH }, _height { HEIGHT }, cursor_x {}, cursor_y {}, textcolor { 0xffff },
      textbgcolor { 0xffff }, textsize { 1 }, rotation {}, wrap { true }, _cp437 { false }, gfxFont { nullptr } {}

void Adafruit_GFX::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    const bool steep { abs(y1 - y0) > abs(x1 - x0) };
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    const int16_t dx { static_cast<int16_t>(x1 - x0) };
    const int16_t dy { static_cast<int16_t>(std::abs(y1 - y0)) };

    int16_t err { static_cast<int16_t>(dx / 2) };
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            writePixel(y0, x0, color);
        } else {
            writePixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void Adafruit_GFX::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    startWrite();
    writeLine(x, y, x, y + h - 1, color);
    endWrite();
}

void Adafruit_GFX::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    startWrite();
    writeLine(x, y, x + w - 1, y, color);
    endWrite();
}

void Adafruit_GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    startWrite();
    for (int16_t i { x }; i < x + w; i++) {
        writeFastVLine(i, y, h, color);
    }
    endWrite();
}

void Adafruit_GFX::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (x0 == x1) {
        if (y0 > y1) {
            std::swap(y0, y1);
        }
        drawFastVLine(x0, y0, y1 - y0 + 1, color);
    } else if (y0 == y1) {
        if (x0 > x1) {
            std::swap(x0, x1);
        }
        drawFastHLine(x0, y0, x1 - x0 + 1, color);
    } else {
        startWrite();
        writeLine(x0, y0, x1, y1, color);
        endWrite();
    }
}

void Adafruit_GFX::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f { static_cast<int16_t>(1 - r) };
    int16_t ddF_x { 1 };
    int16_t ddF_y { static_cast<int16_t>(-2 * r) };
    int16_t x { 0 };
    int16_t y { r };

    startWrite();
    writePixel(x0, y0 + r, color);
    writePixel(x0, y0 - r, color);
    writePixel(x0 + r, y0, color);
    writePixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        writePixel(x0 + x, y0 + y, color);
        writePixel(x0 - x, y0 + y, color);
        writePixel(x0 + x, y0 - y, color);
        writePixel(x0 - x, y0 - y, color);
        writePixel(x0 + y, y0 + x, color);
        writePixel(x0 - y, y0 + x, color);
        writePixel(x0 + y, y0 - x, color);
        writePixel(x0 - y, y0 - x, color);
    }
    endWrite();
}

void Adafruit_GFX::drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f { static_cast<int16_t>(1 - r) };
    int16_t ddF_x { 1 };
    int16_t ddF_y { static_cast<int16_t>(-2 * r) };
    int16_t x { 0 };
    int16_t y { r };

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            writePixel(x0 + x, y0 + y, color);
            writePixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            writePixel(x0 + x, y0 - y, color);
            writePixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            writePixel(x0 - y, y0 + x, color);
            writePixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            writePixel(x0 - y, y0 - x, color);
            writePixel(x0 - x, y0 - y, color);
        }
    }
}

void Adafruit_GFX::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    startWrite();
    writeFastVLine(x0, y0 - r, 2 * r + 1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
    endWrite();
}

void Adafruit_GFX::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {
    int16_t f { static_cast<int16_t>(1 - r) };
    int16_t ddF_x { 1 };
    int16_t ddF_y { static_cast<int16_t>(-2 * r) };
    int16_t x { 0 };
    int16_t y { r };
    int16_t px { x };
    int16_t py { y };

    delta++; // Avoid some +1's in the loop

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        // These checks avoid double-drawing certain lines, important
        // for the SSD1306 library which has an INVERT drawing mode.
        if (x < (y + 1)) {
            if (corners & 1) {
                writeFastVLine(x0 + x, y0 - y, 2 * y + delta, color);
            }
            if (corners & 2) {
                writeFastVLine(x0 - x, y0 - y, 2 * y + delta, color);
            }
        }
        if (y != py) {
            if (corners & 1) {
                writeFastVLine(x0 + py, y0 - px, 2 * px + delta, color);
            }
            if (corners & 2) {
                writeFastVLine(x0 - py, y0 - px, 2 * px + delta, color);
            }
            py = y;
        }
        px = x;
    }
}

void Adafruit_GFX::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    startWrite();
    writeFastHLine(x, y, w, color);
    writeFastHLine(x, y + h - 1, w, color);
    writeFastVLine(x, y, h, color);
    writeFastVLine(x + w - 1, y, h, color);
    endWrite();
}

void Adafruit_GFX::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius { static_cast<int16_t>(((w < h) ? w : h) / 2) }; // 1/2 minor axis
    if (r > max_radius) {
        r = max_radius;
    }
    // smarter version
    startWrite();
    writeFastHLine(x + r, y, w - 2 * r, color); // Top
    writeFastHLine(x + r, y + h - 1, w - 2 * r, color); // Bottom
    writeFastVLine(x, y + r, h - 2 * r, color); // Left
    writeFastVLine(x + w - 1, y + r, h - 2 * r, color); // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1, color);
    drawCircleHelper(x + w - r - 1, y + r, r, 2, color);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
    drawCircleHelper(x + r, y + h - r - 1, r, 8, color);
    endWrite();
}

void Adafruit_GFX::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius { static_cast<int16_t>(((w < h) ? w : h) / 2) }; // 1/2 minor axis
    if (r > max_radius) {
        r = max_radius;
    }
    // smarter version
    startWrite();
    writeFillRect(x + r, y, w - 2 * r, h, color);
    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1, color);
    endWrite();
}

void Adafruit_GFX::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

void Adafruit_GFX::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }
    if (y1 > y2) {
        std::swap(y2, y1);
        std::swap(x2, x1);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }

    startWrite();
    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a) {
            a = x1;
        } else if (x1 > b) {
            b = x1;
        }
        if (x2 < a) {
            a = x2;
        } else if (x2 > b) {
            b = x2;
        }
        writeFastHLine(a, y0, b - a + 1, color);
        endWrite();
        return;
    }

    int16_t dx01 { static_cast<int16_t>(x1 - x0) };
    int16_t dy01 { static_cast<int16_t>(y1 - y0) };
    int16_t dx02 { static_cast<int16_t>(x2 - x0) };
    int16_t dy02 { static_cast<int16_t>(y2 - y0) };
    int16_t dx12 { static_cast<int16_t>(x2 - x1) };
    int16_t dy12 { static_cast<int16_t>(y2 - y1) };
    int32_t sa { 0 };
    int32_t sb { 0 };

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2) {
        last = y1; // Include y1 scanline
    } else {
        last = y1 - 1; // Skip it
    }

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) {
            std::swap(a, b);
        }
        writeFastHLine(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b) {
            std::swap(a, b);
        }
        writeFastHLine(a, y, b - a + 1, color);
    }
    endWrite();
}

void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {
    int16_t byteWidth { static_cast<int16_t>((w + 7) / 8) }; // Bitmap scanline pad = whole byte
    uint8_t byte { 0 };

    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = bitmap[j * byteWidth + i / 8];
            }
            if (byte & 0x80) {
                writePixel(x + i, y, color);
            }
        }
    }
    endWrite();
}

void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg) {
    int16_t byteWidth { static_cast<int16_t>((w + 7) / 8) }; // Bitmap scanline pad = whole byte
    uint8_t byte { 0 };

    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = bitmap[j * byteWidth + i / 8];
            }
            writePixel(x + i, y, (byte & 0x80) ? color : bg);
        }
    }
    endWrite();
}

void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color) {
    int16_t byteWidth { static_cast<int16_t>((w + 7) / 8) }; // Bitmap scanline pad = whole byte
    uint8_t byte { 0 };

    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = bitmap[j * byteWidth + i / 8];
            }
            if (byte & 0x80)
                writePixel(x + i, y, color);
        }
    }
    endWrite();
}

void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg) {
    int16_t byteWidth { static_cast<int16_t>((w + 7) / 8) }; // Bitmap scanline pad = whole byte
    uint8_t byte { 0 };

    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = bitmap[j * byteWidth + i / 8];
            }
            writePixel(x + i, y, (byte & 0x80) ? color : bg);
        }
    }
    endWrite();
}

void Adafruit_GFX::drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color) {
    int16_t byteWidth { static_cast<int16_t>((w + 7) / 8) }; // Bitmap scanline pad = whole byte
    uint8_t byte { 0 };

    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte >>= 1;
            } else {
                byte = bitmap[j * byteWidth + i / 8];
            }
            // Nearly identical to drawBitmap(), only the bit order
            // is reversed here (left-to-right = LSB to MSB):
            if (byte & 0x01) {
                writePixel(x + i, y, color);
            }
        }
    }
    endWrite();
}

void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h) {
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            writePixel(x + i, y, bitmap[j * w + i]);
        }
    }
    endWrite();
}

void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h) {
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            writePixel(x + i, y, bitmap[j * w + i]);
        }
    }
    endWrite();
}

void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h) {
    int16_t bw { static_cast<int16_t>((w + 7) / 8) }; // Bitmask scanline pad = whole byte
    uint8_t byte { 0 };
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = mask[j * bw + i / 8];
            }
            if (byte & 0x80) {
                writePixel(x + i, y, bitmap[j * w + i]);
            }
        }
    }
    endWrite();
}

void Adafruit_GFX::drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t* bitmap, uint8_t* mask, int16_t w, int16_t h) {
    int16_t bw { static_cast<int16_t>((w + 7) / 8) }; // Bitmask scanline pad = whole byte
    uint8_t byte { 0 };
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = mask[j * bw + i / 8];
            }
            if (byte & 0x80) {
                writePixel(x + i, y, bitmap[j * w + i]);
            }
        }
    }
    endWrite();
}

void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) {
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            writePixel(x + i, y, pgm_read_word(&bitmap[j * w + i]));
        }
    }
    endWrite();
}

void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, int16_t w, int16_t h) {
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            writePixel(x + i, y, bitmap[j * w + i]);
        }
    }
    endWrite();
}

void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h) {
    int16_t bw { static_cast<int16_t>((w + 7) / 8) }; // Bitmask scanline pad = whole byte
    uint8_t byte { 0 };
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = mask[j * bw + i / 8];
            }
            if (byte & 0x80) {
                writePixel(x + i, y, bitmap[j * w + i]);
            }
        }
    }
    endWrite();
}

void Adafruit_GFX::drawRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, uint8_t* mask, int16_t w, int16_t h) {
    int16_t bw { static_cast<int16_t>((w + 7) / 8) }; // Bitmask scanline pad = whole byte
    uint8_t byte { 0 };
    startWrite();
    for (int16_t j { 0 }; j < h; j++, y++) {
        for (int16_t i { 0 }; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = mask[j * bw + i / 8];
            }
            if (byte & 0x80) {
                writePixel(x + i, y, bitmap[j * w + i]);
            }
        }
    }
    endWrite();
}

void Adafruit_GFX::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
    if (!gfxFont) { // 'Classic' built-in font
        if ((x >= _width) || // Clip right
            (y >= _height) || // Clip bottom
            ((x + 6 * size - 1) < 0) || // Clip left
            ((y + 8 * size - 1) < 0)) { // Clip top
            return;
        }

        if (!_cp437 && (c >= 176)) {
            c++; // Handle 'classic' charset behavior
        }

        startWrite();
        for (int8_t i { 0 }; i < 5; i++) { // Char bitmap = 5 columns
            uint8_t line { font[c * 5 + i] };
            for (int8_t j { 0 }; j < 8; j++, line >>= 1) {
                if (line & 1) {
                    if (size == 1) {
                        writePixel(x + i, y + j, color);
                    } else {
                        writeFillRect(x + i * size, y + j * size, size, size, color);
                    }
                } else if (bg != color) {
                    if (size == 1) {
                        writePixel(x + i, y + j, bg);
                    } else {
                        writeFillRect(x + i * size, y + j * size, size, size, bg);
                    }
                }
            }
        }
        if (bg != color) { // If opaque, draw vertical line for last column
            if (size == 1) {
                writeFastVLine(x + 5, y, 8, bg);
            } else {
                writeFillRect(x + 5 * size, y, size, 8 * size, bg);
            }
        }
        endWrite();
    } else { // Custom font
        // Character is assumed previously filtered by write() to eliminate
        // newlines, returns, non-printable characters, etc.  Calling
        // drawChar() directly with 'bad' characters of font may cause mayhem!
        c -= gfxFont->first;
        GFXglyph* glyph { &(((GFXglyph*) gfxFont->glyph))[c] };
        uint8_t* bitmap { gfxFont->bitmap };

        uint16_t bo { glyph->bitmapOffset };
        uint8_t w { glyph->width };
        uint8_t h { glyph->height };
        int8_t xo { glyph->xOffset };
        int8_t yo { glyph->yOffset };
        uint8_t xx, yy, bits { 0 }, bit { 0 };
        int16_t xo16 { 0 }, yo16 { 0 };

        if (size > 1) {
            xo16 = xo;
            yo16 = yo;
        }

        // Todo: Add character clipping here

        // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
        // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
        // has typically been used with the 'classic' font to overwrite old
        // screen contents with new data.  This ONLY works because the
        // characters are a uniform size; it's not a sensible thing to do with
        // proportionally-spaced fonts with glyphs of varying sizes (and that
        // may overlap).  To replace previously-drawn text when using a custom
        // font, use the getTextBounds() function to determine the smallest
        // rectangle encompassing a string, erase the area with fillRect(),
        // then draw new text.  This WILL infortunately 'blink' the text, but
        // is unavoidable.  Drawing 'background' pixels will NOT fix this,
        // only creates a new set of problems.  Have an idea to work around
        // this (a canvas object type for MCUs that can afford the RAM and
        // displays supporting setAddrWindow() and pushColors()), but haven't
        // implemented this yet.

        startWrite();
        for (yy = 0; yy < h; yy++) {
            for (xx = 0; xx < w; xx++) {
                if (!(bit++ & 7)) {
                    bits = bitmap[bo++];
                }
                if (bits & 0x80) {
                    if (size == 1) {
                        writePixel(x + xo + xx, y + yo + yy, color);
                    } else {
                        writeFillRect(x + (xo16 + xx) * size, y + (yo16 + yy) * size, size, size, color);
                    }
                }
                bits <<= 1;
            }
        }
        endWrite();
    } // End classic vs custom font
}

size_t Adafruit_GFX::write(uint8_t c) {
    if (!gfxFont) { // 'Classic' built-in font
        if (c == '\n') { // Newline?
            cursor_x = 0; // Reset x to zero,
            cursor_y += textsize * 8; // advance y one line
        } else if (c != '\r') { // Ignore carriage returns
            if (wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
                cursor_x = 0; // Reset x to zero,
                cursor_y += textsize * 8; // advance y one line
            }
            drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
            cursor_x += textsize * 6; // Advance x one char
        }
    } else { // Custom font
        if (c == '\n') {
            cursor_x = 0;
            cursor_y += (int16_t) textsize * gfxFont->yAdvance;
        } else if (c != '\r') {
            const uint8_t first { gfxFont->first };
            if ((c >= first) && (c <= gfxFont->last)) {
                GFXglyph* glyph = &(((GFXglyph*) gfxFont->glyph)[c - first]);
                uint8_t w { glyph->width };
                uint8_t h { glyph->height };
                if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
                    int16_t xo = glyph->xOffset; // sic
                    if (wrap && ((cursor_x + textsize * (xo + w)) > _width)) {
                        cursor_x = 0;
                        cursor_y += (int16_t) textsize * gfxFont->yAdvance;
                    }
                    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
                }
                cursor_x += glyph->xAdvance * (int16_t) textsize;
            }
        }
    }
    return 1;
}

void Adafruit_GFX::setRotation(uint8_t x) {
    rotation = (x & 3);
    switch (rotation) {
        case 0:
            /* no break */
        case 2:
            _width = WIDTH;
            _height = HEIGHT;
            break;
        case 1:
            /* no break */
        case 3:
            _width = HEIGHT;
            _height = WIDTH;
            break;
    }
}

void Adafruit_GFX::setFont(const GFXfont* f) {
    if (f) { // Font struct pointer passed in?
        if (!gfxFont) { // And no current font struct?
            // Switching from classic to new font behavior.
            // Move cursor pos down 6 pixels so it's on baseline.
            cursor_y += 6;
        }
    } else if (gfxFont) { // NULL passed.  Current font struct defined?
        // Switching from new to classic font behavior.
        // Move cursor pos up 6 pixels so it's at top-left of char.
        cursor_y -= 6;
    }
    gfxFont = (GFXfont*) f;
}

void Adafruit_GFX::charBounds(char c, int16_t* x, int16_t* y, int16_t* minx, int16_t* miny, int16_t* maxx, int16_t* maxy) {
    if (gfxFont) {
        if (c == '\n') { // Newline?
            *x = 0; // Reset x to zero, advance y by one line
            *y += textsize * gfxFont->yAdvance;
        } else if (c != '\r') { // Not a carriage return; is normal char
            uint8_t first { gfxFont->first };
            uint8_t last { gfxFont->last };
            if ((c >= first) && (c <= last)) { // Char present in this font?
                GFXglyph* glyph = &(((GFXglyph*) gfxFont->glyph)[c - first]);
                uint8_t gw { glyph->width };
                uint8_t gh { glyph->height };
                uint8_t xa { glyph->xAdvance };
                int8_t xo { glyph->xOffset };
                int8_t yo { glyph->yOffset };
                if (wrap && ((*x + (((int16_t) xo + gw) * textsize)) > _width)) {
                    *x = 0; // Reset x to zero, advance y by one line
                    *y += textsize * (uint8_t) pgm_read_byte(&gfxFont->yAdvance);
                }
                int16_t ts { textsize };
                int16_t x1 { static_cast<int16_t>(*x + xo * ts) };
                int16_t y1 { static_cast<int16_t>(*y + yo * ts) };
                int16_t x2 { static_cast<int16_t>(x1 + gw * ts - 1) };
                int16_t y2 { static_cast<int16_t>(y1 + gh * ts - 1) };
                if (x1 < *minx)
                    *minx = x1;
                if (y1 < *miny)
                    *miny = y1;
                if (x2 > *maxx)
                    *maxx = x2;
                if (y2 > *maxy)
                    *maxy = y2;
                *x += xa * ts;
            }
        }
    } else { // Default font
        if (c == '\n') { // Newline?
            *x = 0; // Reset x to zero,
            *y += textsize * 8; // advance y one line
            // min/max x/y unchaged -- that waits for next 'normal' character
        } else if (c != '\r') { // Normal char; ignore carriage returns
            if (wrap && ((*x + textsize * 6) > _width)) { // Off right?
                *x = 0; // Reset x to zero,
                *y += textsize * 8; // advance y one line
            }
            int x2 { *x + textsize * 6 - 1 }; // Lower-right pixel of char
            int y2 { *y + textsize * 8 - 1 };
            if (x2 > *maxx) {
                *maxx = x2; // Track max x, y
            }
            if (y2 > *maxy) {
                *maxy = y2;
            }
            if (*x < *minx) {
                *minx = *x; // Track min x, y
            }
            if (*y < *miny) {
                *miny = *y;
            }
            *x += textsize * 6; // Advance x one char
        }
    }
}

void Adafruit_GFX::getTextBounds(const char* str, int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h) {
    *x1 = x;
    *y1 = y;
    *w = *h = 0;

    int16_t minx { _width };
    int16_t miny { _height };
    int16_t maxx { -1 };
    int16_t maxy { -1 };

    uint8_t c;
    while ((c = *str++)) {
        charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    }

    if (maxx >= minx) {
        *x1 = minx;
        *w = maxx - minx + 1;
    }
    if (maxy >= miny) {
        *y1 = miny;
        *h = maxy - miny + 1;
    }
}

void Adafruit_GFX::getTextBounds(const String& str, int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h) {
    if (str.length() != 0) {
        getTextBounds(const_cast<char*>(str.c_str()), x, y, x1, y1, w, h);
    }
}


void Adafruit_GFX_Button::initButtonUL(
    Adafruit_GFX* gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char* label, uint8_t textsize) {
    _x1 = x1;
    _y1 = y1;
    _w = w;
    _h = h;
    _outlinecolor = outline;
    _fillcolor = fill;
    _textcolor = textcolor;
    _textsize = textsize;
    _gfx = gfx;
    strncpy(_label, label, 9);
}

void Adafruit_GFX_Button::drawButton(bool inverted) {
    uint16_t fill, outline, text;

    if (!inverted) {
        fill = _fillcolor;
        outline = _outlinecolor;
        text = _textcolor;
    } else {
        fill = _textcolor;
        outline = _outlinecolor;
        text = _fillcolor;
    }

    const uint8_t r { static_cast<uint8_t>(std::min(_w, _h) / 4) }; // Corner radius
    _gfx->fillRoundRect(_x1, _y1, _w, _h, r, fill);
    _gfx->drawRoundRect(_x1, _y1, _w, _h, r, outline);

    _gfx->setCursor(_x1 + (_w / 2) - (strlen(_label) * 3 * _textsize), _y1 + (_h / 2) - (4 * _textsize));
    _gfx->setTextColor(text);
    _gfx->setTextSize(_textsize);
    _gfx->print(_label);
}

bool Adafruit_GFX_Button::contains(int16_t x, int16_t y) const {
    return ((x >= _x1) && (x < static_cast<int16_t>(_x1 + _w)) && (y >= _y1) && (y < static_cast<int16_t>(_y1 + _h)));
}

void Adafruit_GFX_Button::press(bool p) {
    laststate = currstate;
    currstate = p;
}


// GFXcanvas1, GFXcanvas8 and GFXcanvas16 (currently a WIP, don't get too
// comfy with the implementation) provide 1-, 8- and 16-bit offscreen
// canvases, the address of which can be passed to drawBitmap() or
// pushColors() (the latter appears only in a couple of GFX-subclassed TFT
// libraries at this time).  This is here mostly to help with the recently-
// added proportionally-spaced fonts; adds a way to refresh a section of the
// screen without a massive flickering clear-and-redraw...but maybe you'll
// find other uses too.  VERY RAM-intensive, since the buffer is in MCU
// memory and not the display driver...GXFcanvas1 might be minimally useful
// on an Uno-class board, but this and the others are much more likely to
// require at least a Mega or various recent ARM-type boards (recommended,
// as the text+bitmap draw can be pokey).  GFXcanvas1 requires 1 bit per
// pixel (rounded up to nearest byte per scanline), GFXcanvas8 is 1 byte
// per pixel (no scanline pad), and GFXcanvas16 uses 2 bytes per pixel (no
// scanline pad).
// NOT EXTENSIVELY TESTED YET.  MAY CONTAIN WORST BUGS KNOWN TO HUMANKIND.

GFXcanvas1::GFXcanvas1(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
    const size_t bytes { static_cast<size_t>(((w + 7) / 8) * h) };
    buffer = new uint8_t[bytes];

    if (buffer) {
        std::memset(buffer, 0, bytes);
    }
}

GFXcanvas1::~GFXcanvas1() {
    if (buffer) {
        delete[] buffer;
    }
}

void GFXcanvas1::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (!buffer) {
        return;
    }

    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
        return;
    }

    int16_t t;
    switch (rotation) {
        case 1:
            t = x;
            x = WIDTH - 1 - y;
            y = t;
            break;
        case 2:
            x = WIDTH - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
    }

    uint8_t* ptr { &buffer[(x / 8) + y * ((WIDTH + 7) / 8)] };
    if (color) {
        *ptr |= 0x80 >> (x & 7);
    } else {
        *ptr &= ~(0x80 >> (x & 7));
    }
}

void GFXcanvas1::fillScreen(uint16_t color) {
    if (!buffer) {
        return;
    }

    const size_t bytes { static_cast<size_t>(((WIDTH + 7) / 8) * HEIGHT) };
    std::memset(buffer, color ? 0xFF : 0x00, bytes);
}


GFXcanvas8::GFXcanvas8(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
    const size_t bytes { static_cast<size_t>(w * h) };
    buffer = new uint8_t[bytes];

    if (buffer) {
        std::memset(buffer, 0, bytes);
    }
}

GFXcanvas8::~GFXcanvas8() {
    if (buffer) {
        delete[] buffer;
    }
}

void GFXcanvas8::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (!buffer) {
        return;
    }

    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
        return;

    int16_t t;
    switch (rotation) {
        case 1:
            t = x;
            x = WIDTH - 1 - y;
            y = t;
            break;
        case 2:
            x = WIDTH - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
    }

    buffer[x + y * WIDTH] = color;
}

void GFXcanvas8::fillScreen(uint16_t color) {
    if (!buffer) {
        return;
    }

    std::memset(buffer, color, WIDTH * HEIGHT);
}

void GFXcanvas8::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    if (!buffer) {
        return;
    }

    if ((x >= _width) || (y < 0) || (y >= _height)) {
        return;
    }

    const int16_t x2 { static_cast<int16_t>(x + w - 1) };
    if (x2 < 0) {
        return;
    }

    // Clip left/right
    if (x < 0) {
        x = 0;
        w = x2 + 1;
    }

    if (x2 >= _width) {
        w = _width - x;
    }

    int16_t t;
    switch (rotation) {
        case 1:
            t = x;
            x = WIDTH - 1 - y;
            y = t;
            break;
        case 2:
            x = WIDTH - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
    }

    std::memset(buffer + y * WIDTH + x, color, w);
}


GFXcanvas16::GFXcanvas16(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
    const size_t bytes { static_cast<size_t>(w * h * 2) };
    buffer = new uint16_t[bytes / 2];

    if (buffer) {
        std::memset(buffer, 0, bytes);
    }
}

GFXcanvas16::~GFXcanvas16() {
    if (buffer) {
        delete[] buffer;
    }
}

void GFXcanvas16::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (!buffer) {
        return;
    }

    if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
        return;
    }

    int16_t t;
    switch (rotation) {
        case 1:
            t = x;
            x = WIDTH - 1 - y;
            y = t;
            break;
        case 2:
            x = WIDTH - 1 - x;
            y = HEIGHT - 1 - y;
            break;
        case 3:
            t = x;
            x = y;
            y = HEIGHT - 1 - t;
            break;
    }

    buffer[x + y * WIDTH] = color;
}

void GFXcanvas16::fillScreen(uint16_t color) {
    if (!buffer) {
        return;
    }

    const uint8_t hi { static_cast<uint8_t>(color >> 8) };
    const uint8_t lo { static_cast<uint8_t>(color & 0xff) };
    if (hi == lo) {
        std::memset(buffer, lo, WIDTH * HEIGHT * 2);
    } else {
        const uint32_t pixels { static_cast<uint32_t>(WIDTH * HEIGHT) };
        for (uint32_t i { 0 }; i < pixels; i++) {
            buffer[i] = color;
        }
    }
}
