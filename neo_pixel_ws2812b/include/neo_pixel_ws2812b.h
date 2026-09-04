#ifndef NEO_PIXEL_H
#define NEO_PIXEL_H 1

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/rmt_tx.h"

/**
 * @file neo_pixel.h
 * @brief NeoPixel LED driver for ESP32.
 *
 * This module provides an interface for controlling addressable RGB LEDs
 * using the ESP32's RMT (Remote Control) peripheral. It supports organizing
 * pixels in a 2D grid and individual pixel control with RGB colors.
 */

/**
 * @def NEO_PIXEL_COLOR_RGB(r, g, b)
 * @brief Macro to create an RGB color array.
 *
 * Convenience macro that creates a temporary uint8_t array containing
 * red, green, and blue components. Useful for passing colors directly
 * to NeoPixel functions.
 *
 * @param r Red component (0–255).
 * @param g Green component (0–255).
 * @param b Blue component (0–255).
 *
 * @return A temporary uint8_t array: {r, g, b}.
 *
 * @note The returned array is a temporary and should not be stored.
 *       Use it directly as a function argument.
 *
 * @example
 * neo_pixel_fill(&pixel, NEO_PIXEL_COLOR_RGB(255, 0, 0)); // Red
 */
#define NEO_PIXEL_COLOR_RGB(r, g, b) ((uint8_t[]){(r), (g), (b)})

/**
 * @struct neo_pixel_t
 * @brief NeoPixel display controller structure.
 *
 * Contains the RMT peripheral configuration, pixel buffer, and display
 * dimensions. Users should initialize this structure using neo_pixel_init()
 * and not directly access its members.
 *
 * @var neo_pixel_t::rmt_channel_handle
 *      Handle to the RMT channel used for transmitting data.
 *
 * @var neo_pixel_t::rmt_transmit_config
 *      Configuration parameters for RMT transmission.
 *
 * @var neo_pixel_t::pixels
 *      Pointer to the pixel buffer storing GRB values.
 *      Each pixel occupies 3 bytes (green, red, blue).
 *
 * @var neo_pixel_t::pixels_length
 *      Total number of bytes in the pixel buffer.
 *      Equals width × height × 3.
 *
 * @var neo_pixel_t::width
 *      Horizontal dimension of the pixel grid.
 *
 * @var neo_pixel_t::height
 *      Vertical dimension of the pixel grid.
 *
 * @var neo_pixel_t::encoder
 *      Handle to the RMT encoder for NeoPixel protocol encoding.
 */
typedef struct
{
    rmt_channel_handle_t rmt_channel_handle;
    rmt_transmit_config_t rmt_transmit_config;
    uint8_t *pixels;
    size_t pixels_length;
    uint8_t width;
    uint8_t height;
    rmt_encoder_handle_t encoder;
} neo_pixel_t;

/**
 * @brief Initializes a NeoPixel display.
 *
 * Sets up the RMT peripheral, allocates the pixels buffer, and configures
 * the encoder for NeoPixel communication. The pixels buffer is obtained from
 * the caller and must remain valid for the lifetime of the neo_pixel_t object.
 *
 * @param[out]    neo_pixel            Pointer to the NeoPixel structure to initialize.
 * @param[in]     gpio_num             GPIO pin connected to the NeoPixel data line.
 * @param[in,out] encoder_current_pos  Pointer to the encoder's current position tracker.
 *                                      Updated internally by the RMT driver.
 * @param[in]     pixels               Pointer to a pixels buffer of size width × height × 3.
 * @param[in]     width                Horizontal dimension of the pixels grid.
 * @param[in]     height               Vertical dimension of the pixels grid.
 *
 * @return None.
 *
 * @note The pixels buffer is not cleared by this function.
 *       Call neo_pixel_clear() to initialize all pixels to black.
 * @note The caller is responsible for allocating the pixels buffer.
 */
void neo_pixel_init(
    neo_pixel_t *neo_pixel,
    gpio_num_t gpio_num,
    size_t *encoder_current_pos,
    uint8_t *pixels,
    uint8_t width,
    uint8_t height);

/**
 * @brief Transmits the current pixel buffer to the NeoPixel LEDs.
 *
 * Encodes and sends the pixel data to the connected NeoPixel strip via
 * the RMT peripheral. Call this function after modifying pixels to update
 * the physical LED display.
 *
 * @param[in,out] neo_pixel Pointer to the NeoPixel object.
 *
 * @return None.
 *
 * @note This is a blocking operation. Do not call from interrupt handlers.
 */
void neo_pixel_refresh(
    neo_pixel_t *neo_pixel);

/**
 * @brief Fills all pixels with a single color.
 *
 * Sets every pixel in the display to the specified RGB color.
 * Call neo_pixel_refresh() afterward to update the physical LEDs.
 *
 * @param[in,out] neo_pixel  Pointer to the NeoPixel object.
 * @param[in]     color_rgb  RGB color array containing:
 *                           color_rgb[0] = red (0–255),
 *                           color_rgb[1] = green (0–255),
 *                           color_rgb[2] = blue (0–255).
 *
 * @return None.
 *
 * @example
 * neo_pixel_fill(&pixel, NEO_PIXEL_COLOR_RGB(255, 0, 0)); // Fill with red
 * neo_pixel_refresh(&pixel);
 */
void neo_pixel_fill(
    neo_pixel_t *neo_pixel,
    const uint8_t color_rgb[static 3]);

/**
 * @brief Clears all pixels (sets them to black).
 *
 * Sets all pixels to black (0, 0, 0). Call neo_pixel_refresh() afterward
 * to update the physical LEDs.
 *
 * @param[in,out] neo_pixel Pointer to the NeoPixel object.
 *
 * @return None.
 *
 * @example
 * neo_pixel_clear(&pixel);
 * neo_pixel_refresh(&pixel);
 */
void neo_pixel_clear(
    neo_pixel_t *neo_pixel);

/**
 * @brief Sets a pixel color by linear index.
 *
 * Sets the color of a pixel identified by its linear index in the pixel buffer.
 * Use this function when pixels are treated as a linear array.
 * For 2D grid access, use neo_pixel_set_pixel().
 *
 * Call neo_pixel_refresh() to transmit changes to the LEDs.
 *
 * @param[in,out] neo_pixel      Pointer to the NeoPixel object.
 * @param[in]     pixel_index    Linear index of the pixel (0 to width×height−1).
 * @param[in]     color_rgb      RGB color array containing:
 *                               color_rgb[0] = red (0–255),
 *                               color_rgb[1] = green (0–255),
 *                               color_rgb[2] = blue (0–255).
 *
 * @return None.
 *
 * @warning No bounds checking is performed. Ensure pixel_index is valid.
 *
 * @example
 * neo_pixel_set_pixel_by_index(&pixel, 5, NEO_PIXEL_COLOR_RGB(0, 255, 0));
 * neo_pixel_refresh(&pixel);
 */
void neo_pixel_set_pixel_by_index(
    neo_pixel_t *neo_pixel,
    uint16_t pixel_index,
    const uint8_t color_rgb[static 3]);

/**
 * @brief Retrieve the RGB color of a pixel by its linear index.
 *
 * Reads the current RGB color value stored in the pixel buffer at the specified
 * linear index. The linear index maps to a 2D position as:
 * @code
 * linear_index = y * width + x
 * @endcode
 *
 * The retrieved color is copied into the provided @p color_rgb array in RGB order
 * (red at [0], green at [1], blue at [2]).
 *
 * @param[in] neo_pixel     Pointer to the initialized @ref neo_pixel_t structure.
 *                          Must not be NULL.
 * @param[in] pixel_index   Linear index of the pixel to read (0 to width×height−1).
 * @param[out] color_rgb    Pointer to a static array of at least 3 @c uint8_t elements
 *                          to receive the RGB values. Array indices: [0] = red,
 *                          [1] = green, [2] = blue. Values range from 0–255.
 *
 * @warning No bounds checking is performed. If @p pixel_index is greater than or
 *          equal to (width × height), a buffer overread will occur, returning
 *          undefined data. The caller must ensure the index is valid.
 *
 * @note This function does not transmit data to the LEDs. Use @ref neo_pixel_refresh
 *       to update the physical LED strip with the current pixel buffer state.
 *
 * @see neo_pixel_set_pixel_by_index
 * @see neo_pixel_get_pixel
 * @see neo_pixel_refresh
 */
void neo_pixel_get_pixel_by_index(
    const neo_pixel_t *neo_pixel,
    uint16_t pixel_index,
    uint8_t color_rgb[static 3]);

/**
 * @brief Sets a pixel color at 2D coordinates.
 *
 * Sets the pixel at the specified (x, y) coordinates in the 2D grid.
 * Call neo_pixel_refresh() to transmit changes to the LEDs.
 *
 * @param[in,out] neo_pixel  Pointer to the NeoPixel object to modify.
 * @param[in]     x          Horizontal pixel coordinate (0 to width−1).
 * @param[in]     y          Vertical pixel coordinate (0 to height−1).
 * @param[in]     color_rgb  RGB color array containing:
 *                           color_rgb[0] = red (0–255),
 *                           color_rgb[1] = green (0–255),
 *                           color_rgb[2] = blue (0–255).
 *
 * @return None.
 *
 * @warning No bounds checking is performed. Ensure x < width and y < height.
 *
 * @example
 * neo_pixel_set_pixel(&pixel, 2, 3, NEO_PIXEL_COLOR_RGB(255, 128, 0));
 * neo_pixel_refresh(&pixel);
 */
void neo_pixel_set_pixel(
    neo_pixel_t *neo_pixel,
    uint8_t x,
    uint8_t y,
    const uint8_t color_rgb[static 3]);

/**
 * @brief Retrieves the RGB color of a pixel at 2D coordinates.
 *
 * Copies the current color of the pixel at (x, y) into the caller-provided
 * output buffer.
 *
 * @param[in]  neo_pixel  Pointer to the NeoPixel object.
 * @param[in]  x          Horizontal pixel coordinate (0 to width−1).
 * @param[in]  y          Vertical pixel coordinate (0 to height−1).
 * @param[out] color_rgb  Output buffer for the RGB color. Must have space for
 *                        at least 3 bytes:
 *                        color_rgb[0] = red,
 *                        color_rgb[1] = green,
 *                        color_rgb[2] = blue.
 *
 * @return None.
 *
 * @warning No bounds checking is performed. Ensure x < width and y < height.
 * @warning The output buffer must be allocated by the caller with minimum
 *          size of 3 bytes.
 *
 * @example
 * uint8_t color[3];
 * neo_pixel_get_pixel(&pixel, 2, 3, color);
 * printf("RGB: %d, %d, %d\n", color[0], color[1], color[2]);
 */
void neo_pixel_get_pixel(
    const neo_pixel_t *neo_pixel,
    uint8_t x,
    uint8_t y,
    uint8_t color_rgb[static 3]);

#endif
