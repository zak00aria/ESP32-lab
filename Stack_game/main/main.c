#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "multiplexed_leds_matrix.h"
#include "game_of_life.h"

#define COLS_CONTROL_PINS GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21
#define ROWS_CONTROL_PINS GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12
#define LED_MATRIX_W (uint8_t)5
#define LED_MATRIX_H (uint8_t)7

gpio_num_t rows_control_pins_list[LED_MATRIX_H] = {ROWS_CONTROL_PINS};
gpio_num_t cols_control_pins_list[LED_MATRIX_W] = {COLS_CONTROL_PINS};

Multiplexed_LEDs_matrix_t LEDs_matrix = {};

uint8_t current_generation[LED_MATRIX_W * LED_MATRIX_H] = {
    0, 1, 1, 0, 0,
    1, 0, 0, 1, 0,
    1, 0, 0, 0, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0,
    1, 0, 1, 0, 1,
    0, 1, 0, 1, 0};

static void refresh_LEDs_matrix_task(void *arg);
static void advance_game_of_life_task(void *arg);

void app_main(void)
{
    int8_t delay_counter = 0;

    multiplexed_LEDs_matrix_init(
        &LEDs_matrix,
        current_generation,
        LED_MATRIX_W,
        LED_MATRIX_H,
        rows_control_pins_list,
        cols_control_pins_list);

    while (1)
    {

        // advance game of life after updating LEDs matrix 20 times
        delay_counter++;
        if (delay_counter < 20)
            continue;
        delay_counter = 0;
    }
}

