#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#include "nlsf595_leds_matrix.h"
#include "game_of_life.h"

#define GPIO_SI 32
#define GPIO_SCK 26
#define GPIO_RCK 25
#define LED_MATRIX_W (uint8_t)8
#define LED_MATRIX_H (uint8_t)8
#define INIT_CELLS_STATE_LENGTH (uint16_t)12

nlsf595_LEDs_matrix_t LEDs_matrix = {};

Game_of_life_states_t game_of_life_cells[LED_MATRIX_W * LED_MATRIX_H] = {STATE_DEAD};
uint16_t game_of_life_init_cells_state[INIT_CELLS_STATE_LENGTH] = {10, 12, 19, 25, 26, 27, 42, 43, 51, 57, 58, 59};
Game_of_life_t game_of_life = {};

uint8_t current_generation[LED_MATRIX_W * LED_MATRIX_H] = {0};

void app_main(void)
{
    nlsf595_LEDs_matrix_init(
        &LEDs_matrix,
        current_generation,
        LED_MATRIX_W,
        LED_MATRIX_H,
        GPIO_SI,
        GPIO_SCK,
        GPIO_RCK);

    game_of_life_init(
        &game_of_life,
        game_of_life_cells,
        LED_MATRIX_W,
        LED_MATRIX_H,
        game_of_life_init_cells_state,
        INIT_CELLS_STATE_LENGTH);

    while (1)
    {
        // update LEDs matrix buffer
        for (uint8_t y = 0; y < game_of_life.height; y++)
            for (uint8_t x = 0; x < game_of_life.width; x++)
            {
                nlsf595_LEDs_matrix_set(
                    &LEDs_matrix,
                    x,
                    y,
                    game_of_life_get_cell(&game_of_life, x, y) == STATE_DEAD
                        ? MLM_LOW
                        : MLM_HIGHT);
            }

        // update LEDs matrix
        nlsf595_LEDs_matrix_refresh(&LEDs_matrix);

        // advance game of life
        game_of_life_advance(&game_of_life);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
