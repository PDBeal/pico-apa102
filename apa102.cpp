/*!
 * \file apa102.cpp
 *
 * \mainpage APA102 Library
 *
 * \section intro_sec Introduction
 *
 * Example to control AP102 RGBW LED Modules in a strand or strip with RP2040 SDK only
 *
 * \section author Author
 *
 * Written by PBeal
 * 
 * \section license License
 * GNU LESSER GENERAL PUBLIC LICENSE, V3.0
 */
#include "apa102.h"

APA102::APA102(uint16_t num, uint8_t mosi_pin, uint8_t sck_pin, PIO pio, int sm) {
    alloc(num);

    pixelSm = sm;
    pixelPio = pio;
    sckpin = sck_pin;
    mosipin = mosi_pin;
}

APA102::APA102(uint16_t num, uint8_t mosi_pin, uint8_t sck_pin) {
    alloc(num);
    
    PIO pio = pio0;
    int sm;
    // Find a free SM on one of the PIO's
    sm = pio_claim_unused_sm(pio, false); // don't panic
    // Try pio1 if SM not found
    if (sm < 0) {
        pio = pio1;
        sm = pio_claim_unused_sm(pio, true); // panic if no SM is free
    }
    
    pixelSm = sm;
    pixelPio = pio;
    sckpin = sck_pin;
    mosipin = mosi_pin;
}

// Allocate 3 bytes per pixel, init to RGB 'off' state:
void APA102::alloc(uint16_t num)
{
    numLEDs = ((pixels = (uint8_t *)calloc(num, 3)) != NULL) ? num : 0;
}

// Release memory (as needed):
APA102::~APA102(void)
{
    if (pixels)
        free(pixels);
}

void APA102::begin() {
    uint offset = pio_add_program(pixelPio, &apa102_mini_program);
    apa102_mini_program_init(pixelPio, pixelSm, offset, 5 * 1000 * 1000, sckpin, mosipin);
}

void APA102::put_start_frame(PIO pio, uint sm) {
    pio_sm_put_blocking(pio, sm, 0u);
}

void APA102::put_end_frame(PIO pio, uint sm) {
    pio_sm_put_blocking(pio, sm, ~0u);
}

void APA102::put_rgb888(PIO pio, uint sm, uint8_t r, uint8_t g, uint8_t b) {
    pio_sm_put_blocking(pio, sm,
                        0x7 << 29 |                   // magic
                        (brightness & 0x1f) << 24 |   // global brightness parameter
                        (uint32_t) b << 16 |
                        (uint32_t) g << 8 |
                        (uint32_t) r << 0
    );
}

uint16_t APA102::numPixels(void) { return numLEDs; }

void APA102::show(void)
{
    uint16_t i;
    const uint8_t *ledParamPtr = &ledParam;

    put_start_frame(pixelPio, pixelSm);
    // Write 24 bits per pixel:
    for (i = 0; i < numLEDs; i++)
    {
        uint8_t redPtr = pixels[i*3];
        uint8_t greenPtr = pixels[(i*3)+1];
        uint8_t bluePtr = pixels[(i*3)+2];

        put_rgb888(pixelPio, pixelSm, redPtr, greenPtr, bluePtr);
    };
    put_end_frame(pixelPio, pixelSm);
    sleep_ms(10);

    //sleep_ms(1); // Data is latched by holding clock pin low for 1 millisecond
}

// Set pixel color from separate 8-bit R, G, B components:
void APA102::setPixelColor(uint16_t led, uint8_t red, uint8_t green,
                                 uint8_t blue)
{
    if (led < numLEDs)
    {
        uint8_t *p = &pixels[led * 3];
        *p++ = red;
        *p++ = green;
        *p++ = blue;
    }
}

// Set pixel color from 'packed' 32-bit RGB value:
void APA102::setPixelColor(uint16_t led, uint32_t color)
{
    if (led < numLEDs)
    {
        uint8_t *p = &pixels[led * 3];
        *p++ = color >> 16; // Red
        *p++ = color >> 8;  // Green
        *p++ = color;       // Blue
    }
}

// Clear all pixels
void APA102::clear()
{
    if (pixels != NULL)
    {
        memset(pixels, 0, numLEDs * 3);
    }
}

// Query color from previously-set pixel (returns packed 32-bit RGB value)
uint32_t APA102::getPixelColor(uint16_t led)
{
    if (led < numLEDs)
    {
        uint16_t ofs = led * 3;
        // To keep the show() loop as simple & fast as possible, the
        // internal color representation is native to different pixel
        // types.  For compatibility with existing code, 'packed' RGB
        // values passed in or out are always 0xRRGGBB order.
        return ((uint32_t)pixels[ofs] << 16) | ((uint16_t)pixels[ofs + 1] << 8) | ((uint16_t)pixels[ofs + 2]);
    }

    return 0; // Pixel # is out of bounds
}

