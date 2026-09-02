/**
 * @file multiplexed_leds_matrix.h
 * @brief Multiplexed LED Matrix driver for ESP32
 *
 * This module provides functions to initialize and control a multiplexed LED matrix.
 * It handles GPIO configuration, matrix refresh, and individual LED control.
 */

#ifndef MULTIPLEXED_LEDs_MATRIX_H
#define MULTIPLEXED_LEDs_MATRIX_H 1

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

/**
 * @struct Multiplexed_LEDs_matrix_t
 * @brief Configuration and state structure for a multiplexed LED matrix
 *
 * This structure holds the matrix dimensions, LED state buffer, and GPIO pin information
 * required to control a multiplexed LED matrix display.
 */
typedef struct
{
    uint8_t width;                      ///< Width of the LED matrix (number of columns)
    uint8_t height;                     ///< Height of the LED matrix (number of rows)
    uint8_t *LEDs_state;                ///< Pointer to array storing the state of each LED
    gpio_num_t *rows_control_pins_list; ///< Pointer to array of GPIO pins controlling rows
    gpio_num_t *cols_control_pins_list; ///< Pointer to array of GPIO pins controlling columns
} Multiplexed_LEDs_matrix_t;

/**
 * @enum Multiplexed_LEDs_matrix_level_t
 * @brief Enumeration for LED brightness levels
 */
typedef enum
{
    MLM_LOW,   ///< LED is OFF (low level)
    MLM_HIGHT, ///< LED is ON (high level)
} Multiplexed_LEDs_matrix_level_t;

/**
 * @brief Initializes a multiplexed LED matrix
 *
 * Configures all GPIO pins as outputs and sets up the multiplexed LED matrix structure.
 * This function must be called before using any other matrix functions.
 *
 * @param[in,out] multiplexed_LEDs_matrix  Pointer to the matrix structure to initialize
 * @param[in]     LEDs_state              Pointer to array storing LED states (must be width * height bytes)
 * @param[in]     width                   Number of columns (LED pins) in the matrix
 * @param[in]     height                  Number of rows (LED pins) in the matrix
 * @param[in]     rows_control_pins_list  Array of GPIO pin numbers for row control
 * @param[in]     cols_control_pins_list  Array of GPIO pin numbers for column control
 */
void multiplexed_LEDs_matrix_init(
    Multiplexed_LEDs_matrix_t *multiplexed_LEDs_matrix,
    uint8_t *LEDs_state,
    uint8_t width,
    uint8_t height,
    gpio_num_t *rows_control_pins_list,
    gpio_num_t *cols_control_pins_list);

/**
 * @brief Refreshes the LED matrix display
 *
 * Updates the physical LED matrix by scanning through each column and updating
 * the row states accordingly. This function uses multiplexing to control the display.
 * Should be called regularly to maintain a persistent display.
 *
 * @param[in] LEDs_matrix  Pointer to the matrix structure to refresh
 */
void multiplexed_LEDs_matrix_refresh(Multiplexed_LEDs_matrix_t *LEDs_matrix);

/**
 * @brief Sets the state of an LED at a specific position
 *
 * Updates the state of an individual LED at the given (x, y) coordinates
 * within the matrix. The change takes effect on the next refresh.
 *
 * @param[in,out] LEDs_matrix  Pointer to the matrix structure
 * @param[in]     x            Column index (starting from 0)
 * @param[in]     y            Row index (starting from 0)
 * @param[in]     level        The level to set (MLM_LOW or MLM_HIGHT)
 */
void multiplexed_LEDs_matrix_set(
    Multiplexed_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y,
    Multiplexed_LEDs_matrix_level_t level);

/**
 * @brief Gets the state of an LED at a specific position
 *
 * Retrieves the current state of an individual LED at the given (x, y) coordinates
 * within the matrix.
 *
 * @param[in] LEDs_matrix  Pointer to the matrix structure
 * @param[in] x            Column index (0 to width-1)
 * @param[in] y            Row index (0 to height-1)
 *
 * @return The level of the LED (MLM_LOW or MLM_HIGHT)
 */
Multiplexed_LEDs_matrix_level_t multiplexed_LEDs_matrix_get(
    Multiplexed_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y);

/**
 * @brief Resets all LEDs in the matrix to OFF state
 *
 * Sets all LEDs in the matrix to MLM_LOW (OFF). The display will be blank
 * after the next refresh.
 *
 * @param[in,out] LEDs_matrix  Pointer to the matrix structure to reset
 */
void multiplexed_LEDs_matrix_clear(Multiplexed_LEDs_matrix_t *LEDs_matrix);

#endif