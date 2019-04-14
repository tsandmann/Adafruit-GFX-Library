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

#pragma once

#include "Arduino.h"
#include "Print.h"
#include "gfxfont.h"

/// A generic graphics superclass that can handle all sorts of drawing. At a minimum you can subclass and provide drawPixel(). At a maximum you can do a ton of
/// overriding to optimize.
class Adafruit_GFX : public Print {
public:
    /*!
       @brief    Instatiate a GFX context for graphics! Can only be done by a superclass
       @param    w   Display width, in pixels
       @param    h   Display height, in pixels
    */
    Adafruit_GFX(int16_t w, int16_t h);

    virtual ~Adafruit_GFX() = default;

    virtual void drawPixel(
        int16_t x, int16_t y, uint16_t color) = 0; ///< Virtual drawPixel() function to draw to the screen/framebuffer/etc, must be overridden in subclass.
                                                   ///< @param x X coordinate.  @param y Y coordinate. @param color 16-bit pixel color.

    /*!
       @brief    Start a display-writing routine, overwrite in subclasses.
    */
    virtual void startWrite() {}

    /*!
        @brief   Write a pixel, overwrite in subclasses if startWrite is defined!
        @param   x   x coordinate
        @param   y   y coordinate
        @param   color 16-bit 5-6-5 Color to fill with
    */
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) {
        drawPixel(x, y, color);
    }

    /*!
        @brief    Write a rectangle completely with one color, overwrite in subclasses if startWrite is defined!
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    w   Width in pixels
        @param    h   Height in pixels
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        fillRect(x, y, w, h, color);
    }

    /*!
        @brief    Write a perfectly vertical line, overwrite in subclasses if startWrite is defined!
        @param    x   Top-most x coordinate
        @param    y   Top-most y coordinate
        @param    h   Height in pixels
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
        drawFastVLine(x, y, h, color);
    }

    /*!
        @brief    Write a perfectly horizontal line, overwrite in subclasses if startWrite is defined!
        @param    x   Left-most x coordinate
        @param    y   Left-most y coordinate
        @param    w   Width in pixels
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
        drawFastHLine(x, y, w, color);
    }

    /*!
        @brief    Write a line.  Bresenham's algorithm - thx wikpedia
        @param    x0  Start point x coordinate
        @param    y0  Start point y coordinate
        @param    x1  End point x coordinate
        @param    y1  End point y coordinate
        @param    color 16-bit 5-6-5 Color to draw with
    */
    virtual void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

    /*!
        @brief    End a display-writing routine, overwrite in subclasses if startWrite is defined!
    */
    virtual void endWrite() {}

    /*!
        @brief      Set rotation setting for display
        @param  x   0 thru 3 corresponding to 4 cardinal rotations
    */
    virtual void setRotation(uint8_t r);

    /*!
        @brief   Invert the display (ideally using built-in hardware command)
        @param   True if you want to invert, false to make 'normal'
    */
    virtual void invertDisplay(bool) {
        // Do nothing, must be subclassed if supported by hardware
    }

    /*!
        @brief    Draw a perfectly vertical line (this is often optimized in a subclass!)
        @param    x   Top-most x coordinate
        @param    y   Top-most y coordinate
        @param    h   Height in pixels
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);

    /*!
        @brief    Draw a perfectly horizontal line (this is often optimized in a subclass!)
        @param    x   Left-most x coordinate
        @param    y   Left-most y coordinate
        @param    w   Width in pixels
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

    /*!
        @brief    Fill a rectangle completely with one color. Update in subclasses if desired!
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    w   Width in pixels
        @param    h   Height in pixels
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    /*!
        @brief    Fill the screen completely with one color. Update in subclasses if desired!
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void fillScreen(uint16_t color) {
        fillRect(0, 0, _width, _height, color);
    }

    /*!
        @brief    Draw a line. Update in subclasses if desired!
        @param    x0  Start point x coordinate
        @param    y0  Start point y coordinate
        @param    x1  End point x coordinate
        @param    y1  End point y coordinate
        @param    color 16-bit 5-6-5 Color to draw with
    */
    virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

    /*!
        @brief    Draw a rectangle with no fill color
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    w   Width in pixels
        @param    h   Height in pixels
        @param    color 16-bit 5-6-5 Color to draw with
    */
    virtual void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    /*!
        @brief    Draw a circle outline
        @param    x0   Center-point x coordinate
        @param    y0   Center-point y coordinate
        @param    r   Radius of circle
        @param    color 16-bit 5-6-5 Color to draw with
    */
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

    /*!
        @brief    Quarter-circle drawer, used to do circles and roundrects
        @param    x0   Center-point x coordinate
        @param    y0   Center-point y coordinate
        @param    r   Radius of circle
        @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
        @param    color 16-bit 5-6-5 Color to draw with
    */
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);

    /*!
        @brief    Draw a circle with filled color
        @param    x0   Center-point x coordinate
        @param    y0   Center-point y coordinate
        @param    r   Radius of circle
        @param    color 16-bit 5-6-5 Color to fill with
    */
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

    /*!
        @brief  Quarter-circle drawer with fill, used for circles and roundrects
        @param  x0       Center-point x coordinate
        @param  y0       Center-point y coordinate
        @param  r        Radius of circle
        @param  corners  Mask bits indicating which quarters we're doing
        @param  delta    Offset from center-point, used for round-rects
        @param  color    16-bit 5-6-5 Color to fill with
    */
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);

    /*!
        @brief    Draw a triangle with no fill color
        @param    x0  Vertex #0 x coordinate
        @param    y0  Vertex #0 y coordinate
        @param    x1  Vertex #1 x coordinate
        @param    y1  Vertex #1 y coordinate
        @param    x2  Vertex #2 x coordinate
        @param    y2  Vertex #2 y coordinate
        @param    color 16-bit 5-6-5 Color to draw with
    */
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

    /*!
        @brief    Draw a triangle with color-fill
        @param    x0  Vertex #0 x coordinate
        @param    y0  Vertex #0 y coordinate
        @param    x1  Vertex #1 x coordinate
        @param    y1  Vertex #1 y coordinate
        @param    x2  Vertex #2 x coordinate
        @param    y2  Vertex #2 y coordinate
        @param    color 16-bit 5-6-5 Color to fill/draw with
    */
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

    /*!
        @brief    Draw a rounded rectangle with no fill color
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    w   Width in pixels
        @param    h   Height in pixels
        @param    r   Radius of corner rounding
        @param    color 16-bit 5-6-5 Color to draw with
    */
    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

    /*!
        @brief    Draw a rounded rectangle with fill color
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    w   Width in pixels
        @param    h   Height in pixels
        @param    r   Radius of corner rounding
        @param    color 16-bit 5-6-5 Color to draw/fill with
    */
    void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

    /*!
        @brief    Draw a 1-bit image at the specified (x,y) position, using the specified foreground color (unset bits are transparent).
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with monochrome bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Hieght of bitmap in pixels
        @param    color 16-bit 5-6-5 Color to draw with
    */
    void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);

    /*!
        @brief    Draw a 1-bit image at the specified (x,y) position, using the specified foreground (for set bits) and background (unset bits) colors.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with monochrome bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Hieght of bitmap in pixels
        @param    color 16-bit 5-6-5 Color to draw pixels with
        @param    bg 16-bit 5-6-5 Color to draw background with
    */
    void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg);

    /*!
        @brief    Draw a RAM-resident 1-bit image at the specified (x,y) position, using the specified foreground color (unset bits are transparent).
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with monochrome bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Hieght of bitmap in pixels
        @param    color 16-bit 5-6-5 Color to draw with
    */
    void drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color);

    /*!
        @brief    Draw a 1-bit image at the specified (x,y) position, using the specified foreground (for set bits) and background (unset bits) colors.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with monochrome bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Hieght of bitmap in pixels
        @param    color 16-bit 5-6-5 Color to draw pixels with
        @param    bg 16-bit 5-6-5 Color to draw background with
    */
    void drawBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bg);

    /*!
        @brief    Draw PXBitMap Files (*.xbm), exported from GIMP. Usage: Export from GIMP to *.xbm, rename *.xbm to *.c and open in editor.
                  C Array can be directly used with this function. There is no RAM-resident version of this function; if generating bitmaps
                  in RAM, use the format defined by drawBitmap() and call that instead.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with monochrome bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Hieght of bitmap in pixels
        @param    color 16-bit 5-6-5 Color to draw pixels with
    */
    void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color);

    /*!
        @brief    Draw a 8-bit image (grayscale) at the specified (x,y) pos.
                  Specifically for 8-bit display devices such as IS31FL3731; no color reduction/expansion is performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with grayscale bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Hieght of bitmap in pixels
    */
    void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h);

    /*!
        @brief    Draw a RAM-resident 8-bit image (grayscale) at the specified (x,y) pos.
                  Specifically for 8-bit display devices such as IS31FL3731; no color reduction/expansion is performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with grayscale bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Hieght of bitmap in pixels
    */
    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t* bitmap, int16_t w, int16_t h);

    /*!
        @brief    Draw a PROGMEM-resident 8-bit image (grayscale) with a 1-bit mask
                  (set bits = opaque, unset bits = clear) at the specified (x,y) position.
                  Specifically for 8-bit display devices such as IS31FL3731; no color reduction/expansion is performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with grayscale bitmap
        @param    mask  byte array with mask bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Height of bitmap in pixels
    */
    void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);

    /*!
        @brief    Draw a RAM-resident 8-bit image (grayscale) with a 1-bit mask
                  (set bits = opaque, unset bits = clear) at the specified (x,y) position.
                  Specifically for 8-bit display devices such as IS31FL3731; no color reduction/expansion is performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with grayscale bitmap
        @param    mask  byte array with mask bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Height of bitmap in pixels
    */
    void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t* bitmap, uint8_t* mask, int16_t w, int16_t h);

    /*!
        @brief    Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) at the specified (x,y) position.
                  For 16-bit display devices; no color reduction performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with 16-bit color bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Height of bitmap in pixels
    */
    void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);

    /*!
        @brief    Draw a RAM-resident 16-bit image (RGB 5/6/5) at the specified (x,y) position.
                  For 16-bit display devices; no color reduction performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with 16-bit color bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Height of bitmap in pixels
    */
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, int16_t w, int16_t h);

    /*!
        @brief    Draw a PROGMEM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask (set bits = opaque, unset bits = clear) at the specified (x,y) position.
                  For 16-bit display devices; no color reduction performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with 16-bit color bitmap
        @param    mask  byte array with monochrome mask bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Height of bitmap in pixels
    */
    void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], const uint8_t mask[], int16_t w, int16_t h);

    /*!
        @brief    Draw a RAM-resident 16-bit image (RGB 5/6/5) with a 1-bit mask (set bits = opaque, unset bits = clear) at the specified (x,y) position.
                  For 16-bit display devices; no color reduction performed.
        @param    x   Top left corner x coordinate
        @param    y   Top left corner y coordinate
        @param    bitmap  byte array with 16-bit color bitmap
        @param    mask  byte array with monochrome mask bitmap
        @param    w   Width of bitmap in pixels
        @param    h   Height of bitmap in pixels
    */
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t* bitmap, uint8_t* mask, int16_t w, int16_t h);

    /*!
        @brief    Draw a single character
        @param    x   Bottom left corner x coordinate
        @param    y   Bottom left corner y coordinate
        @param    c   The 8-bit font-indexed character (likely ascii)
        @param    color 16-bit 5-6-5 Color to draw chraracter with
        @param    bg 16-bit 5-6-5 Color to fill background with (if same as color, no background)
        @param    size  Font magnification level, 1 is 'original' size
    */
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

    /*!
        @brief  Set text cursor location
        @param  x    X coordinate in pixels
        @param  y    Y coordinate in pixels
    */
    void setCursor(int16_t x, int16_t y) {
        cursor_x = x;
        cursor_y = y;
    }

    /*!
        @brief   Set text font color with transparant background
        @param   c   16-bit 5-6-5 Color to draw text with
    */
    void setTextColor(uint16_t c) {
        // For 'transparent' background, we'll set the bg to the same as fg instead of using a flag
        textcolor = textbgcolor = c;
    }

    /*!
        @brief   Set text font color with custom background color
        @param   c   16-bit 5-6-5 Color to draw text with
        @param   b   16-bit 5-6-5 Color to draw background/fill with
    */
    void setTextColor(uint16_t c, uint16_t b) {
        textcolor = c;
        textbgcolor = b;
    }

    /*!
        @brief   Set text 'magnification' size. Each increase in s makes 1 pixel that much bigger.
        @param  s  Desired text size. 1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
    */
    void setTextSize(uint8_t s) {
        textsize = (s > 0) ? s : 1;
    }

    /*!
        @brief      Whether text that is too long should 'wrap' around to the next line.
        @param  w Set true for wrapping, false for clipping
    */
    void setTextWrap(bool w) {
        wrap = w;
    }

    /*!
        @brief  Enable (or disable) Code Page 437-compatible charset.
                There was an error in glcdfont.c for the longest time -- one character
                (#176, the 'light shade' block) was missing -- this threw off the index
                of every character that followed it.  But a TON of code has been written
                with the erroneous character indices.  By default, the library uses the
                original 'wrong' behavior and old sketches will still work.  Pass 'true'
                to this function to use correct CP437 character values in your code.
        @param  x  Whether to enable (True) or not (False)
    */
    void cp437(bool x = true) {
        _cp437 = x;
    }

    /*!
        @brief Set the font to display when print()ing, either custom or default
        @param  f  The GFXfont object, if NULL use built in 6x8 font
    */
    void setFont(const GFXfont* f = nullptr);

    /*!
        @brief    Helper to determine size of a string with current font/size. Pass string and a cursor position, returns UL corner and W,H.
        @param    str     The ascii string to measure
        @param    x       The current cursor X
        @param    y       The current cursor Y
        @param    x1      The boundary X coordinate, set by function
        @param    y1      The boundary Y coordinate, set by function
        @param    w      The boundary width, set by function
        @param    h      The boundary height, set by function
    */
    void getTextBounds(const char* string, int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h);

    /*!
        @brief    Helper to determine size of a string with current font/size. Pass string and a cursor position, returns UL corner and W,H.
        @param    str    The ascii string to measure (as an arduino String() class)
        @param    x      The current cursor X
        @param    y      The current cursor Y
        @param    x1     The boundary X coordinate, set by function
        @param    y1     The boundary Y coordinate, set by function
        @param    w      The boundary width, set by function
        @param    h      The boundary height, set by function
    */
    void getTextBounds(const String& str, int16_t x, int16_t y, int16_t* x1, int16_t* y1, uint16_t* w, uint16_t* h);

    /*!
        @brief  Print one byte/character of data, used to support print()
        @param  c  The 8-bit ascii character to write
    */
    virtual size_t write(uint8_t c);

    /*!
        @brief      Get height of the display, accounting for the current rotation
        @returns    Height in pixels
    */
    int16_t height() const {
        return _height;
    }

    /*!
        @brief      Get width of the display, accounting for the current rotation
        @returns    Width in pixels
    */
    int16_t width() const {
        return _width;
    }

    /*!
        @brief      Get rotation setting for display
        @returns    0 thru 3 corresponding to 4 cardinal rotations
    */
    uint8_t getRotation() const {
        return rotation;
    }

    /*!
        @brief  Get text cursor X location
        @returns    X coordinate in pixels
    */
    int16_t getCursorX() const {
        return cursor_x;
    }

    /*!
        @brief      Get text cursor Y location
        @returns    Y coordinate in pixels
    */
    int16_t getCursorY() const {
        return cursor_y;
    }

protected:
    /*!
        @brief    Helper to determine size of a character with current font/size.
                  Broke this out as it's used by both the PROGMEM- and RAM-resident getTextBounds() functions.
        @param    c     The ascii character in question
        @param    x     Pointer to x location of character
        @param    y     Pointer to y location of character
        @param    minx  Minimum clipping value for X
        @param    miny  Minimum clipping value for Y
        @param    maxx  Maximum clipping value for X
        @param    maxy  Maximum clipping value for Y
    */
    void charBounds(char c, int16_t* x, int16_t* y, int16_t* minx, int16_t* miny, int16_t* maxx, int16_t* maxy);

    const int16_t WIDTH; ///< This is the 'raw' display width - never changes
    const int16_t HEIGHT; ///< This is the 'raw' display height - never changes
    int16_t _width; ///< Display width as modified by current rotation
    int16_t _height; ///< Display height as modified by current rotation
    int16_t cursor_x; ///< x location to start print()ing text
    int16_t cursor_y; ///< y location to start print()ing text
    uint16_t textcolor; ///< 16-bit background color for print()
    uint16_t textbgcolor; ///< 16-bit text color for print()
    uint8_t textsize; ///< Desired magnification of text to print()
    uint8_t rotation; ///< Display rotation (0 thru 3)
    bool wrap; ///< If set, 'wrap' text at right edge of display
    bool _cp437; ///< If set, use correct CP437 charset (default is off)
    GFXfont* gfxFont; ///< Pointer to special font
};


/// A simple drawn button UI element
class Adafruit_GFX_Button {
public:
    /*!
    @brief    Create a simple drawn button UI element
    */
    Adafruit_GFX_Button() : _gfx {}, currstate { false }, laststate { false } {}

    /*!
        @brief    Initialize button with our desired color/size/settings
        @param    gfx     Pointer to our display so we can draw to it!
        @param    x       The X coordinate of the center of the button
        @param    y       The Y coordinate of the center of the button
        @param    w       Width of the buttton
        @param    h       Height of the buttton
        @param    outline  Color of the outline (16-bit 5-6-5 standard)
        @param    fill  Color of the button fill (16-bit 5-6-5 standard)
        @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
        @param    label  Ascii string of the text inside the button
        @param    textsize The font magnification of the label text
        @note     Classic initButton() function: pass center & size
    */
    void initButton(
        Adafruit_GFX* gfx, int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char* label, uint8_t textsize) {
        // Tweak arguments and pass to the newer initButtonUL() function...
        initButtonUL(gfx, x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor, label, textsize);
    }

    /*!
        @brief    Initialize button with our desired color/size/settings, with upper-left coordinates
        @param    gfx     Pointer to our display so we can draw to it!
        @param    x1       The X coordinate of the Upper-Left corner of the button
        @param    y1       The Y coordinate of the Upper-Left corner of the button
        @param    w       Width of the buttton
        @param    h       Height of the buttton
        @param    outline  Color of the outline (16-bit 5-6-5 standard)
        @param    fill  Color of the button fill (16-bit 5-6-5 standard)
        @param    textcolor  Color of the button label (16-bit 5-6-5 standard)
        @param    label  Ascii string of the text inside the button
        @param    textsize The font magnification of the label text
    */
    void initButtonUL(
        Adafruit_GFX* gfx, int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char* label, uint8_t textsize);

    /*!
        @brief    Draw the button on the screen
        @param    inverted Whether to draw with fill/text swapped to indicate 'pressed'
    */
    void drawButton(bool inverted = false);

    /*!
        @brief    Helper to let us know if a coordinate is within the bounds of the button
        @param    x       The X coordinate to check
        @param    y       The Y coordinate to check
        @returns  True if within button graphics outline
    */
    bool contains(int16_t x, int16_t y) const;

    /*!
        @brief    Sets the state of the button, should be done by some touch function
        @param    p  True for pressed, false for not.
    */
    void press(bool p);

    /*!
       @brief    Query whether the button is currently pressed
       @returns  True if pressed
    */
    bool isPressed() const {
        return currstate;
    }

    /*!
        @brief    Query whether the button was pressed since we last checked state
        @returns  True if was not-pressed before, now is.
    */
    bool justPressed() const {
        return (currstate && !laststate);
    }

    /*!
        @brief    Query whether the button was released since we last checked state
        @returns  True if was pressed before, now is not.
    */
    bool justReleased() const {
        return (!currstate && laststate);
    }

private:
    Adafruit_GFX* _gfx;
    int16_t _x1, _y1; // Coordinates of top-left corner
    uint16_t _w, _h;
    uint8_t _textsize;
    uint16_t _outlinecolor, _fillcolor, _textcolor;
    char _label[10];
    bool currstate, laststate;
};


/// A GFX 1-bit canvas context for graphics
class GFXcanvas1 : public Adafruit_GFX {
public:
    /*!
        @brief    Instatiate a GFX 1-bit canvas context for graphics
        @param    w   Display width, in pixels
        @param    h   Display height, in pixels
    */
    GFXcanvas1(uint16_t w, uint16_t h);

    /*!
        @brief    Delete the canvas, free memory
    */
    virtual ~GFXcanvas1() override;

    /*!
        @brief    Draw a pixel to the canvas framebuffer
        @param   x   x coordinate
        @param   y   y coordinate
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    /*!
        @brief    Fill the framebuffer completely with one color
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void fillScreen(uint16_t color) override;

    /*!
        @brief    Get a pointer to the internal buffer memory
        @returns  A pointer to the allocated buffer
    */
    uint8_t* getBuffer() {
        return buffer;
    }

private:
    uint8_t* buffer;
};


/// A GFX 8-bit canvas context for graphics
class GFXcanvas8 : public Adafruit_GFX {
public:
    /*!
        @brief    Instatiate a GFX 8-bit canvas context for graphics
        @param    w   Display width, in pixels
        @param    h   Display height, in pixels
    */
    GFXcanvas8(uint16_t w, uint16_t h);

    /*!
        @brief    Delete the canvas, free memory
    */
    virtual ~GFXcanvas8() override;

    /*!
        @brief   Draw a pixel to the canvas framebuffer
        @param   x   x coordinate
        @param   y   y coordinate
        @param   color 16-bit 5-6-5 Color to fill with
    */
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    /*!
        @brief    Fill the framebuffer completely with one color
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void fillScreen(uint16_t color) override;

    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;

    /*!
        @brief    Get a pointer to the internal buffer memory
        @returns  A pointer to the allocated buffer
    */
    uint8_t* getBuffer() {
        return buffer;
    }

private:
    uint8_t* buffer;
};


///  A GFX 16-bit canvas context for graphics
class GFXcanvas16 : public Adafruit_GFX {
public:
    /*!
        @brief    Instatiate a GFX 16-bit canvas context for graphics
        @param    w   Display width, in pixels
        @param    h   Display height, in pixels
    */
    GFXcanvas16(uint16_t w, uint16_t h);

    /*!
    @brief    Delete the canvas, free memory
    */
    virtual ~GFXcanvas16() override;

    /*!
        @brief   Draw a pixel to the canvas framebuffer
        @param   x   x coordinate
        @param   y   y coordinate
        @param   color 16-bit 5-6-5 Color to fill with
    */
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    /*!
        @brief    Fill the framebuffer completely with one color
        @param    color 16-bit 5-6-5 Color to fill with
    */
    virtual void fillScreen(uint16_t color) override;

    /*!
        @brief    Get a pointer to the internal buffer memory
        @returns  A pointer to the allocated buffer
    */
    uint16_t* getBuffer() {
        return buffer;
    }

private:
    uint16_t* buffer;
};
