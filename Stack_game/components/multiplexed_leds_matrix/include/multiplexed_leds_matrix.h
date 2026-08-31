#ifndef MULTIPLEXED_LEDs_MATRIX_H
#define MULTIPLEXED_LEDs_MATRIX_H 1

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

typedef struct
{
    uint8_t width;
    uint8_t height;
    uint8_t *LEDs_state;
    gpio_num_t *rows_control_pins_list;
    gpio_num_t *cols_control_pins_list;
} Multiplexed_LEDs_matrix_t;

typedef enum
{
    MLM_LOW,
    MLM_HIGHT,
} Multiplexed_LEDs_matrix_level_t;

void multiplexed_LEDs_matrix_init(
    Multiplexed_LEDs_matrix_t *multiplexed_LEDs_matrix,
    uint8_t *LEDs_state,
    uint8_t width,
    uint8_t height,
    gpio_num_t *rows_control_pins_list,
    gpio_num_t *cols_control_pins_list);

void multiplexed_LEDs_matrix_refresh(Multiplexed_LEDs_matrix_t *LEDs_matrix);

void multiplexed_LEDs_matrix_set(
    Multiplexed_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y,
    Multiplexed_LEDs_matrix_level_t level);

/**
 * Get an LED level by (x, y)
 */
Multiplexed_LEDs_matrix_level_t multiplexed_LEDs_matrix_get(
    Multiplexed_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y);

/**
 * Set all matrix LEDs level to LOW (turn off)
 */
void multiplexed_LEDs_matrix_reset(Multiplexed_LEDs_matrix_t *LEDs_matrix);

#endif