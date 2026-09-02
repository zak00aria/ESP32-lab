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
#define INIT_CELLS_STATE_LENGTH (uint16_t)6

gpio_num_t rows_control_pins_list[LED_MATRIX_H] = {ROWS_CONTROL_PINS};
gpio_num_t cols_control_pins_list[LED_MATRIX_W] = {COLS_CONTROL_PINS};
Multiplexed_LEDs_matrix_t LEDs_matrix = {};

Game_of_life_states_t game_of_life_cells[LED_MATRIX_W * LED_MATRIX_H] = {STATE_DEAD};
// uint16_t game_of_life_init_cells_state[INIT_CELLS_STATE_LENGTH] = {4, 5, 6, 7, 11, 12, 16, 17, 21, 22, 26, 27, 31, 32};
uint16_t game_of_life_init_cells_state[INIT_CELLS_STATE_LENGTH] = {11, 12, 16, 21, 22, 23};
Game_of_life_t game_of_life = {};

uint8_t current_generation[LED_MATRIX_W * LED_MATRIX_H] = {0};

static void refresh_LEDs_matrix_task(void *arg);
static void advance_game_of_life_task(void *arg);

void app_main(void)
{
    multiplexed_LEDs_matrix_init(
        &LEDs_matrix,
        current_generation,
        LED_MATRIX_W,
        LED_MATRIX_H,
        rows_control_pins_list,
        cols_control_pins_list);

    game_of_life_init(
        &game_of_life,
        game_of_life_cells,
        LED_MATRIX_W,
        LED_MATRIX_H,
        game_of_life_init_cells_state,
        INIT_CELLS_STATE_LENGTH);

    xTaskCreate(refresh_LEDs_matrix_task, "refresh_LEDs_matrix_task", 1024, NULL, 1, NULL);
    xTaskCreate(advance_game_of_life_task, "advance_game_of_life_task", 1024, NULL, 2, NULL);
}

static void refresh_LEDs_matrix_task(void *arg)
{
    while (1)
    {
        // update LEDs matrix
        for (uint8_t y = 0; y < game_of_life.height; y++)
            for (uint8_t x = 0; x < game_of_life.width; x++)
            {
                multiplexed_LEDs_matrix_set(
                    &LEDs_matrix,
                    x,
                    y,
                    game_of_life_get_cell(&game_of_life, x, y) == STATE_DEAD
                        ? MLM_LOW
                        : MLM_HIGHT);
            }
        multiplexed_LEDs_matrix_refresh(&LEDs_matrix);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

static void advance_game_of_life_task(void *arg)
{
    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // advance game of life
        game_of_life_advance(&game_of_life);
    }
}
