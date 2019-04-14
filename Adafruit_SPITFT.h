/*!
 * @file Adafruit_SPITFT.h
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
 *
 * Written by Limor "ladyada" Fried for Adafruit Industries,
 * with contributions from the open source community.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 * Adapted for use with the c't-Bot teensy framework and ported to C++14 by Timo Sandmann
 */

#pragma once

#include "Arduino.h"
#include "SPI.h"
#include "Adafruit_GFX.h"


/*!
 * @brief  Adafruit_SPITFT is an intermediary class between Adafruit_GFX
 *         and various hardware-specific subclasses for different displays.
 *         It handles certain operations that are common to a range of
 *         displays (address window, area fills, etc.). Originally these were
 *         all color TFT displays interfaced via SPI, but it's since expanded
 *         to include color OLEDs and parallel-interfaced TFTs. THE NAME HAS
 *         BEEN KEPT TO AVOID BREAKING A LOT OF SUBCLASSES AND EXAMPLE CODE.
 *         Many of the class member functions similarly live on with names
 *         that don't necessarily accurately describe what they're doing,
 *         again to avoid breaking a lot of other code. If in doubt, read
 *         the comments.
 */
class Adafruit_SPITFT : public Adafruit_GFX {
public:
    /*!
     *   @brief   Adafruit_SPITFT constructor for hardware SPI using the board's
     *            default SPI peripheral.
     *   @param   w     Display width in pixels at default rotation setting (0).
     *   @param   h     Display height in pixels at default rotation setting (0).
     *   @param   cs    Arduino pin # for chip-select (-1 if unused, tie CS low).
     *   @param   dc    Arduino pin # for data/command select (required).
     *   @param   rst   Arduino pin # for display reset (optional, display reset
     *                  can be tied to MCU reset, default of -1 means unused).
     *   @return  Adafruit_SPITFT object.
     *   @note    Output pins are not initialized; application typically will
     *            need to call subclass' begin() function, which in turn calls
     *            this library's initSPI() function to initialize pins.
     */
    Adafruit_SPITFT(uint16_t w, uint16_t h, int8_t cs, int8_t dc, int8_t rst = -1);

    /*!
     *   @brief   Adafruit_SPITFT constructor for hardware SPI using a specific
     *            SPI peripheral.
     *   @param   w         Display width in pixels at default rotation (0).
     *   @param   h         Display height in pixels at default rotation (0).
     *   @param   spiClass  Pointer to SPIClass type (e.g. &SPI or &SPI1).
     *   @param   cs        Arduino pin # for chip-select (-1 if unused, tie CS low).
     *   @param   dc        Arduino pin # for data/command select (required).
     *   @param   rst       Arduino pin # for display reset (optional, display reset
     *                   can be tied to MCU reset, default of -1 means unused).
     *   @return  Adafruit_SPITFT object.
     *   @note    Output pins are not initialized; application typically will
     *            need to call subclass' begin() function, which in turn calls
     *            this library's initSPI() function to initialize pins.
     */
    Adafruit_SPITFT(uint16_t w, uint16_t h, SPIClass* spiClass, int8_t cs, int8_t dc, int8_t rst = -1);

    /*!
     *   @brief  Display-specific initialization function.
     *   @param  freq  SPI frequency, in hz (or 0 for default or unused).
     */
    virtual void begin(uint32_t freq) = 0;

    /*!
     *   @brief  Set up the specific display hardware's "address window"
     *           for subsequent pixel-pushing operations.
     *   @param  x  Leftmost pixel of area to be drawn (MUST be within
     *              display bounds at current rotation setting).
     *   @param  y  Topmost pixel of area to be drawn (MUST be within
     *              display bounds at current rotation setting).
     *   @param  w  Width of area to be drawn, in pixels (MUST be >0 and,
     *              added to x, within display bounds at current rotation).
     *   @param  h  Height of area to be drawn, in pixels (MUST be >0 and,
     *              added to x, within display bounds at current rotation).
     */
    virtual void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) const = 0;

    /*!
     * @brief  Call before issuing command(s) or data to display. Performs
     *         chip-select (if required) and starts an SPI transaction (if
     *         using hardware SPI and transactions are supported). Required
     *         for all display types; not an SPI-specific function.
     */
    virtual void startWrite() override {
        _spi->beginTransaction(_spi_settings);
        ::digitalWriteFast(_cs, 0);
    }

    /*!
     * @brief  Call after issuing command(s) or data to display. Performs
     *         chip-deselect (if required) and ends an SPI transaction (if
     *         using hardware SPI and transactions are supported). Required
     *         for all display types; not an SPI-specific function.
     */
    virtual void endWrite() override {
        ::digitalWriteFast(_cs, 1);
        _spi->endTransaction();
    }

    /*!
     *   @brief  Draw a single pixel to the display at requested coordinates.
     *           Not self-contained; should follow a startWrite() call.
     *   @param  x      Horizontal position (0 = left).
     *   @param  y      Vertical position   (0 = top).
     *   @param  color  16-bit pixel color in '565' RGB format.
     */
    virtual void writePixel(int16_t x, int16_t y, uint16_t color) override;

    /*!
     *   @brief  Issue a series of pixels from memory to the display. Not self-
     *           contained; should follow startWrite() and setAddrWindow() calls.
     *   @param  colors     Pointer to array of 16-bit pixel values in '565' RGB
     *                      format.
     *   @param  len        Number of elements in 'colors' array.
     *   @param  block      If true (default case if unspecified), function blocks
     *                      until DMA transfer is complete. This is simply IGNORED
     *                      if DMA is not enabled.
     *   @param  bigEndian  If using DMA, and if set true, bitmap in memory is in
     *                      big-endian order (most significant byte first).
     */
    void writePixels(uint16_t* colors, uint32_t len, bool block = true, bool bigEndian = false) const;

    /*!
     *   @brief  Issue a series of pixels, all the same color. Not self-
     *           contained; should follow startWrite() and setAddrWindow() calls.
     *   @param  color  16-bit pixel color in '565' RGB format.
     *   @param  len    Number of pixels to draw.
     */
    void writeColor(uint16_t color, uint32_t len);

    /*!
     *   @brief  Draw a filled rectangle to the display. Not self-contained;
     *           should follow startWrite(). Typically used by higher-level
     *           graphics primitives; user code shouldn't need to call this and
     *           is likely to use the self-contained fillRect() instead.
     *           writeFillRect() performs its own edge clipping and rejection;
     *           see writeFillRectPreclipped() for a more 'raw' implementation.
     *   @param  x      Horizontal position of first corner.
     *   @param  y      Vertical position of first corner.
     *   @param  w      Rectangle width in pixels (positive = right of first
     *               corner, negative = left of first corner).
     *   @param  h      Rectangle height in pixels (positive = below first
     *               corner, negative = above first corner).
     *   @param  color  16-bit fill color in '565' RGB format.
     *   @note   Written in this deep-nested way because C by definition will
     *           optimize for the 'if' case, not the 'else' -- avoids branches
     *           and rejects clipped rectangles at the least-work possibility.
     */
    virtual void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

    /*!
     *   @brief  Draw a horizontal line on the display. Performs edge clipping
     *           and rejection. Not self-contained; should follow startWrite().
     *           Typically used by higher-level graphics primitives; user code
     *           shouldn't need to call this and is likely to use the self-
     *           contained drawFastHLine() instead.
     *   @param  x      Horizontal position of first point.
     *   @param  y      Vertical position of first point.
     *   @param  w      Line width in pixels (positive = right of first point,
     *               negative = point of first corner).
     *   @param  color  16-bit line color in '565' RGB format.
     */
    virtual void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;

    /*!
     *   @brief  Draw a vertical line on the display. Performs edge clipping and
     *           rejection. Not self-contained; should follow startWrite().
     *           Typically used by higher-level graphics primitives; user code
     *           shouldn't need to call this and is likely to use the self-
     *           contained drawFastVLine() instead.
     *   @param  x      Horizontal position of first point.
     *   @param  y      Vertical position of first point.
     *   @param  h      Line height in pixels (positive = below first point,
     *               negative = above first point).
     *   @param  color  16-bit line color in '565' RGB format.
     */
    virtual void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;

    /*!
     *   @brief  A lower-level version of writeFillRect(). This version requires
     *           all inputs are in-bounds, that width and height are positive,
     *           and no part extends offscreen. NO EDGE CLIPPING OR REJECTION IS
     *           PERFORMED. If higher-level graphics primitives are written to
     *           handle their own clipping earlier in the drawing process, this
     *           can avoid unnecessary function calls and repeated clipping
     *           operations in the lower-level functions.
     *   @param  x      Horizontal position of first corner. MUST BE WITHIN SCREEN BOUNDS.
     *   @param  y      Vertical position of first corner. MUST BE WITHIN SCREEN BOUNDS.
     *   @param  w      Rectangle width in pixels. MUST BE POSITIVE AND NOT EXTEND OFF SCREEN.
     *   @param  h      Rectangle height in pixels. MUST BE POSITIVE AND NOT EXTEND OFF SCREEN.
     *   @param  color  16-bit fill color in '565' RGB format.
     *   @note   This is a new function, no graphics primitives besides rects
     *           and horizontal/vertical lines are written to best use this yet.
     */
    void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    /*
     * These functions are similar to the 'write' functions above, but with
     * a chip-select and/or SPI transaction built-in. They're typically used
     * solo -- that is, as graphics primitives in themselves, not invoked by
     * higher-level primitives (which should use the functions above).
     */

    /*!
     *   @brief  Draw a single pixel to the display at requested coordinates.
     *           Self-contained and provides its own transaction as needed
     *           (see writePixel(x,y,color) for a lower-level variant).
     *           Edge clipping is performed here.
     *   @param  x      Horizontal position (0 = left).
     *   @param  y      Vertical position   (0 = top).
     *   @param  color  16-bit pixel color in '565' RGB format.
     */
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    /*!
     *   @brief  Draw a filled rectangle to the display. Self-contained and
     *           provides its own transaction as needed (see writeFillRect() or
     *           writeFillRectPreclipped() for lower-level variants). Edge
     *           clipping and rejection is performed here.
     *   @param  x      Horizontal position of first corner.
     *   @param  y      Vertical position of first corner.
     *   @param  w      Rectangle width in pixels (positive = right of first
     *                  corner, negative = left of first corner).
     *   @param  h      Rectangle height in pixels (positive = below first
     *                  corner, negative = above first corner).
     *   @param  color  16-bit fill color in '565' RGB format.
     *   @note   This repeats the writeFillRect() function almost in its entirety,
     *           with the addition of a transaction start/end. It's done this way
     *           (rather than starting the transaction and calling writeFillRect()
     *           to handle clipping and so forth) so that the transaction isn't
     *           performed at all if the rectangle is rejected. It's really not
     *           that much code.
     */
    virtual void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

    /*!
     *   @brief  Draw a horizontal line on the display. Self-contained and
     *           provides its own transaction as needed (see writeFastHLine() for
     *           a lower-level variant). Edge clipping and rejection is performed here.
     *   @param  x      Horizontal position of first point.
     *   @param  y      Vertical position of first point.
     *   @param  w      Line width in pixels (positive = right of first point,
     *                  negative = point of first corner).
     *   @param  color  16-bit line color in '565' RGB format.
     *   @note   This repeats the writeFastHLine() function almost in its
     *           entirety, with the addition of a transaction start/end. It's
     *           done this way (rather than starting the transaction and calling
     *           writeFastHLine() to handle clipping and so forth) so that the
     *           transaction isn't performed at all if the line is rejected.
     */
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;

    /*!
     *   @brief  Draw a vertical line on the display. Self-contained and provides
     *           its own transaction as needed (see writeFastHLine() for a lower-
     *           level variant). Edge clipping and rejection is performed here.
     *   @param  x      Horizontal position of first point.
     *   @param  y      Vertical position of first point.
     *   @param  h      Line height in pixels (positive = below first point,
     *                  negative = above first point).
     *   @param  color  16-bit line color in '565' RGB format.
     *   @note   This repeats the writeFastVLine() function almost in its
     *           entirety, with the addition of a transaction start/end. It's
     *           done this way (rather than starting the transaction and calling
     *           writeFastVLine() to handle clipping and so forth) so that the
     *           transaction isn't performed at all if the line is rejected.
     */
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;

    /*!
     *   @brief  Essentially writePixel() with a transaction around it. I don't
     *           think this is in use by any of our code anymore (believe it was
     *           for some older BMP-reading examples), but is kept here in case
     *           any user code relies on it. Consider it DEPRECATED.
     *   @param  color  16-bit pixel color in '565' RGB format.
     *   @note   Provided for backward compatibility, consider it deprecated.
     */
    void pushColor(uint16_t color);

    using Adafruit_GFX::drawRGBBitmap; // Check base class first
    /*!
     *   @brief  Draw a 16-bit image (565 RGB) at the specified (x,y) position.
     *           For 16-bit display devices; no color reduction performed.
     *           Adapted from https://github.com/PaulStoffregen/ILI9341_t3
     *           by Marc MERLIN. See examples/pictureEmbed to use this.
     *           5/6/2017: function name and arguments have changed for
     *           compatibility with current GFX library and to avoid naming
     *           problems in prior implementation.  Formerly drawBitmap() with
     *           arguments in different order. Handles its own transaction and
     *           edge clipping/rejection.
     *   @param  x        Top left corner horizontal coordinate.
     *   @param  y        Top left corner vertical coordinate.
     *   @param  pcolors  Pointer to 16-bit array of pixel values.
     *   @param  w        Width of bitmap in pixels.
     *   @param  h        Height of bitmap in pixels.
     */
    void drawRGBBitmap(int16_t x, int16_t y, uint16_t* pcolors, int16_t w, int16_t h);

    /*!
     *   @brief   Given 8-bit red, green and blue values, return a 'packed'
     *            16-bit color value in '565' RGB format (5 bits red, 6 bits
     *            green, 5 bits blue). This is just a mathematical operation,
     *            no hardware is touched.
     *   @param   red    8-bit red brightnesss (0 = off, 255 = max).
     *   @param   green  8-bit green brightnesss (0 = off, 255 = max).
     *   @param   blue   8-bit blue brightnesss (0 = off, 255 = max).
     *   @return  'Packed' 16-bit color value (565 format).
     */
    static constexpr uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) {
        return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
    }

protected:
    /*!
     *   @brief  Configure microcontroller pins for TFT interfacing. Typically
     *           called by a subclass' begin() function.
     *   @param  freq  SPI frequency when using hardware SPI.
     *           If default is passed, will fall back on a device-specific value.
     */
    void initSPI(uint32_t freq = SPI_DEFAULT_FREQ);

    /*!
     * @brief  Write a single command byte to the display. Chip-select and
     *         transaction must have been previously set -- this ONLY sets
     *         the device to COMMAND mode, issues the byte and then restores
     *         DATA mode. There is no corresponding explicit writeData()
     *         function -- just use spiWrite().
     * @param  cmd  8-bit command to write.
     */
    void writeCommand(uint8_t cmd) const {
        SPI_DC_LOW();
        spiWrite(cmd);
        SPI_DC_HIGH();
    }

    /*!
     * @brief   Read a single 8-bit value from the display. Chip-select and
     *          transaction must have been previously set -- this ONLY reads
     *          the byte. This is another of those functions in the library
     *          with a now-not-accurate name that's being maintained for
     *          compatibility with outside code.
     * @return  Unsigned 8-bit value read.
     */
    uint8_t spiRead() const {
        return _spi->transfer((uint8_t) 0);
    }

    /*!
     * @brief  Issue a single 8-bit value to the display. Chip-select,
     *         transaction and data/command selection must have been
     *         previously set -- this ONLY issues the byte. This is another of
     *         those functions in the library with a now-not-accurate name
     *         that's being maintained for compatibility with outside code.
     * @param  b  8-bit value to write.
     */
    void spiWrite(uint8_t b) const {
        _spi->transfer(b);
    }

    /*!
     * @brief  Issue a single 16-bit value to the display. Chip-select,
     *         transaction and data/command selection must have been
     *         previously set -- this ONLY issues the word. Despite the name,
     *         this function is used even if display connection is parallel;
     *         name was maintaned for backward compatibility. Naming is also
     *         not consistent with the 8-bit version, spiWrite(). Sorry about
     *         that. Again, staying compatible with outside code.
     * @param  w  16-bit value to write.
     */
    void SPI_WRITE16(uint16_t w) const {
        _spi->transfer16(w);
    }

    /*!
     * @brief  Issue a single 32-bit value to the display. Chip-select,
     *         transaction and data/command selection must have been
     *         previously set -- this ONLY issues the longword. Despite the
     *         name, this function is used even if display connection is
     *         parallel; name was maintaned for backward compatibility. Naming
     *         is also not consistent with the 8-bit version, spiWrite().
     *         Sorry about that. Again, staying compatible with outside code.
     * @param  l  32-bit value to write.
     */
    void SPI_WRITE32(uint32_t l) const {
        _spi->transfer16(l >> 16);
        _spi->transfer16(l);
    }

    void SPI_DC_LOW() const {
        ::digitalWriteFast(_dc, 0);
    }

    void SPI_DC_HIGH() const {
        ::digitalWriteFast(_dc, 1);
    }

    SPIClass* _spi; /*!< SPI class pointer */
    SPISettings _spi_settings; /*!< SPI transaction settings */

    const int8_t _rst; /*!< Reset pin # (or -1) */
    const int8_t _cs; /*!< Chip select pin # (or -1) */
    const int8_t _dc; /*!< Data/command pin # */

private:
    static constexpr uint32_t SPI_DEFAULT_FREQ { 24'000'000 };
    static constexpr uint32_t SPI_BLOCKSIZE { 32 };

    uint16_t _spi_buffer[SPI_BLOCKSIZE];
};
