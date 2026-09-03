
/**
 * @file nlsf595_leds_matrix.h
 * @brief Driver for controlling an LED matrix using NLSF595 shift registers
 *
 * This header defines the interface for managing an LED matrix
 * controlled by NLSF595 (or similar) shift registers via GPIO pins.
 * The matrix supports setting individual LEDs on/off and refreshing the display.
 */

#ifndef MULTIPLEXED_LEDs_MATRIX_H
#define MULTIPLEXED_LEDs_MATRIX_H 1

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

/**
 * @struct nlsf595_LEDs_matrix_t
 * @brief Configuration and state structure for an NLSF595-based LED matrix
 */
typedef struct
{
    uint8_t width;                      ///< Width of the LED matrix (number of columns)
    uint8_t height;                     ///< Height of the LED matrix (number of rows)
    uint8_t *LEDs_state;                ///< Pointer to array storing the state of each LED
    gpio_num_t SI;                      ///< Serial Input (data line) GPIO pin
    gpio_num_t SCK;                     ///< Shift Clock GPIO pin
    gpio_num_t RCK;                     ///< Register Clock (latch) GPIO pin
} nlsf595_LEDs_matrix_t;

/**
 * @enum nlsf595_LEDs_matrix_level_t
 * @brief LED state level values
 */
typedef enum
{
    MLM_LOW,   ///< LED is OFF (low level)
    MLM_HIGHT, ///< LED is ON (high level)
} nlsf595_LEDs_matrix_level_t;

/**
 * @brief Initialize the LED matrix with GPIO pins and dimensions
 *
 * Configures the specified GPIO pins as outputs and sets up the matrix
 * structure with the given dimensions and LED state buffer.
 *
 * @param[out] LEDs_matrix Pointer to the LED matrix structure to initialize
 * @param[in]  LEDs_state  Pointer to a buffer for storing LED states (must be allocated)
 * @param[in]  width       Number of columns in the matrix
 * @param[in]  height      Number of rows in the matrix
 * @param[in]  SI          GPIO pin number for serial input (data line)
 * @param[in]  SCK         GPIO pin number for shift clock
 * @param[in]  RCK         GPIO pin number for register clock (latch)
 */
void nlsf595_LEDs_matrix_init(
    nlsf595_LEDs_matrix_t *LEDs_matrix,
    uint8_t *LEDs_state,
    uint8_t width,
    uint8_t height,
    gpio_num_t SI,
    gpio_num_t SCK,
    gpio_num_t RCK);

/**
 * @brief Refresh/update the physical LED matrix with current state
 *
 * Sends the entire LED state buffer to the shift registers and latches
 * the data to update the displayed LED matrix.
 *
 * @param[in] LEDs_matrix Pointer to the LED matrix structure
 */
void nlsf595_LEDs_matrix_refresh(nlsf595_LEDs_matrix_t *LEDs_matrix);

/**
 * @brief Set the state of a specific LED in the matrix
 *
 * Updates the state of an LED at the given coordinates. The changes
 * are stored in the internal buffer but not displayed until
 * nlsf595_LEDs_matrix_refresh() is called.
 *
 * @param[in] LEDs_matrix Pointer to the LED matrix structure
 * @param[in] x           Column index (0 to width-1)
 * @param[in] y           Row index (0 to height-1)
 * @param[in] level       LED state (MLM_LOW for off, MLM_HIGHT for on)
 */
void nlsf595_LEDs_matrix_set(
    nlsf595_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y,
    nlsf595_LEDs_matrix_level_t level);

/**
 * @brief Get the current state of a specific LED in the matrix
 *
 * Retrieves the state of an LED at the given coordinates from the
 * internal buffer (not the physical display state).
 *
 * @param[in] LEDs_matrix Pointer to the LED matrix structure
 * @param[in] x           Column index (0 to width-1)
 * @param[in] y           Row index (0 to height-1)
 *
 * @return LED state (MLM_LOW or MLM_HIGHT), or MLM_LOW if coordinates are out of bounds
 */
nlsf595_LEDs_matrix_level_t nlsf595_LEDs_matrix_get(
    nlsf595_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y);

/**
 * @brief Clear all LEDs in the matrix (set all to off state)
 *
 * Sets all LEDs in the internal buffer to MLM_LOW. Call
 * nlsf595_LEDs_matrix_refresh() afterwards to update the display.
 *
 * @param[in] LEDs_matrix Pointer to the LED matrix structure
 */
void nlsf595_LEDs_matrix_clear(nlsf595_LEDs_matrix_t *LEDs_matrix);

/**
 * @brief Send raw data to the shift register via serial interface
 *
 * Shifts data bits into the shift register using the SI (serial input)
 * and SCK (shift clock) pins. Data bits are sent MSB first.
 *
 * @param[in] data        Pointer to the data bytes to send
 * @param[in] data_length Number of bytes to send
 * @param[in] SI          GPIO pin number for serial input
 * @param[in] SCK         GPIO pin number for shift clock
 */
void nlsf595_send_data(const uint8_t *data, const uint8_t data_length, gpio_num_t SI, gpio_num_t SCK);

/**
 * @brief Latch the shift register data to the output pins
 *
 * Pulses the RCK (register clock/latch) pin to transfer data from the
 * shift register to the output latches.
 *
 * @param[in] RCK GPIO pin number for register clock (latch)
 */
void nlsf595_save_data(gpio_num_t RCK);

#endif