#pragma once
/*!
 * \file apa102.h
 */

#include "hardware/pio.h"
#include "pico/stdlib.h"
#include <cstdlib>
#include <string.h>
#include <math.h>

#include "apa102.pio.h"

class APA102
{

public:
    /*!
     * \brief Constructor for use with SPI instance
     *
     * \param num How many LEDs there are
     * \param mosi_pin Master Out Slave In pin
     * \param sck_pin Clock pin
     * \param pio: pio selected
     * \param sm: state machine selected
     */
    APA102(uint16_t num, uint8_t mosi_pin, uint8_t sck_pin, PIO pio, int sm);

    /*!
     * \brief Constructor
     *
     * \param num: Number of pixels in the string
     * \param mosi_pin Master Out Slave In pin
     * \param sck_pin Clock pin
     * 
     * This constructor tries to autoselect pio and sm
     */
    APA102(uint16_t num, uint8_t mosi_pin, uint8_t sck_pin);

    /*!
     * \brief Release memory (as needed):
     */
    ~APA102();

    /*!
     * \brief Activate SPI
     */
    void begin(void);

    /*!
     * \brief Shows the pixels
     */
    void show(void);

    /*!
     * \brief Returns the number of pixels currently connected
     *
     * \return Returns the number of connected pixels
     */
    uint16_t numPixels(void);

    /*!
     * \brief Set pixel color from separate 8-bit R, G, B components
     *
     * \param led Pixel to set
     * \param red Red value
     * \param green Green value
     * \param blue Blue value
     */
    void setPixelColor(uint16_t led, uint8_t red, uint8_t green, uint8_t blue);

    /*!
     * \brief Set pixel color from 'packed' 32-bit RGB value
     *
     * \param led Pixel to set
     * \param color packed 32-bit RGB value to set the pixel
     */
    void setPixelColor(uint16_t led, uint32_t color);

    /*!
     * \brief Clear the entire string
     */
    void clear();

    /*!
     * \brief Query color from previously-set pixel
     * \param n Pixel to query
     * \return Returns packed 32-bit RGB value
     */
    uint32_t getPixelColor(uint16_t n);

private:
    uint16_t numLEDs; // number of pixels
    uint8_t *pixels,  // Holds color values for each LED (3 bytes each)
        sckpin, mosipin; // Clock & data pin numbers

    uint8_t startFrame[4], endFrame[4];
    uint8_t ledParam = 0xFF;

    // pio - 0 or 1
    PIO pixelPio;
    // pio state machine to use
    int pixelSm;

    // SPI instance specifier, either spi0 or spi1
    //spi_inst_t *spi_interface;

    void alloc(uint16_t n);

    // Global brightness value 0->31
    uint brightness = 16;

    void put_start_frame(PIO pio, uint sm);

    void put_end_frame(PIO pio, uint sm);

    void put_rgb888(PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b);

};