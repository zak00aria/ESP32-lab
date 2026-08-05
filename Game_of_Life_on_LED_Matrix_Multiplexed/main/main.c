#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#define CONTROL_PINS_LEN (5 + 7)
#define CONTROL_PINS  GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21
#define LED_MATRIX_W 5
#define LED_MATRIX_H 7

gpio_num_t control_pins_list[CONTROL_PINS_LEN] = {CONTROL_PINS};

uint8_t buffer[LED_MATRIX_W * LED_MATRIX_H] = {
    0, 1, 1, 0, 0, 
    1, 0, 0, 1, 0, 
    1, 0, 0, 0, 1, 
    0, 1, 0, 1, 0, 
    0, 0, 1, 0, 0, 
    1, 0, 1, 0, 1, 
    0, 1, 0, 1, 0
};

uint8_t next_generation[LED_MATRIX_W * LED_MATRIX_H] = { 0 };

void setup_gpio(void);
void update_ledMatrix(void);
void advance_game_of_life(void);
uint8_t count_alive_neighbors(uint8_t* current_generation, uint8_t x, uint8_t y, uint16_t cell_index);

void app_main(void)
{
    int8_t delay_counter = 0;

    setup_gpio();

    while (1)
    {
        update_ledMatrix();

        delay_counter++;
        if (delay_counter < 20)
            continue;
        delay_counter = 0;

        advance_game_of_life();
    }
}

void setup_gpio(void)
{
    gpio_config_t pGPIOConfig = {};
    pGPIOConfig.mode = GPIO_MODE_OUTPUT;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.intr_type = GPIO_INTR_DISABLE;
    pGPIOConfig.pin_bit_mask = 0;
    for (uint8_t i = 0; i < CONTROL_PINS_LEN; i++)
    {
        pGPIOConfig.pin_bit_mask |= 1ULL << control_pins_list[i];
    }
    gpio_config(&pGPIOConfig);
    for (uint8_t i = LED_MATRIX_H; i < CONTROL_PINS_LEN; i++)
    {
        gpio_set_level(control_pins_list[i], 1);
    }
}

void update_ledMatrix(void)
{
    uint16_t i = 0;
    for (int8_t x = 0; x < LED_MATRIX_W; x++)
    {
        for (int8_t y = 0; y < LED_MATRIX_H; y++)
        {
            i = y * LED_MATRIX_W + x;
            gpio_set_level(control_pins_list[y], buffer[i]);
        }
        gpio_set_level(control_pins_list[x + LED_MATRIX_H], 0);
        vTaskDelay(25 / portTICK_PERIOD_MS);
        gpio_set_level(control_pins_list[x + LED_MATRIX_H], 1);
    }
}

uint8_t count_alive_neighbors(uint8_t* current_generation, uint8_t x, uint8_t y, uint16_t cell_index)
{
    uint8_t alive_neighbors_count = 0;
    // left nighbor
    if (x > 0)
    {
        alive_neighbors_count += current_generation[cell_index - 1];
    }

    // left top nighbor
    if (x > 0 && y > 0)
    {
        alive_neighbors_count += current_generation[cell_index - LED_MATRIX_W - 1];
    }

    // right nighbor
    if (x < LED_MATRIX_W - 1)
    {
        alive_neighbors_count += current_generation[cell_index + 1];
    }

    // right top nighbor
    if (x < LED_MATRIX_W - 1 && y > 0)
    {
        alive_neighbors_count += current_generation[cell_index - LED_MATRIX_W + 1];
    }

    // top nighbor
    if (y > 0)
    {
        alive_neighbors_count += current_generation[cell_index - LED_MATRIX_W];
    }

    // bottom nighbor
    if (y < LED_MATRIX_H - 1)
    {
        alive_neighbors_count += current_generation[cell_index + LED_MATRIX_W];
    }
    // left bottom nighbor
    if (x > 0 && y < LED_MATRIX_H - 1)
    {
        alive_neighbors_count += current_generation[cell_index + LED_MATRIX_W - 1];
    }

    // right bottom nighbor
    if (x < LED_MATRIX_W - 1 && y < LED_MATRIX_H - 1)
    {
        alive_neighbors_count += current_generation[cell_index + LED_MATRIX_W + 1];
    }

    return alive_neighbors_count;
}

void advance_game_of_life(void)
{
    uint16_t cell_index = 0;
    uint8_t alive_neighbors_count = 0;

    for (uint8_t y = 0; y < LED_MATRIX_H; y++)
    {
        for (uint8_t x = 0; x < LED_MATRIX_W; x++)
        {
            alive_neighbors_count = count_alive_neighbors(buffer, x, y, cell_index);
            next_generation[cell_index] = (buffer[cell_index] && alive_neighbors_count == 2) || alive_neighbors_count == 3;
            cell_index++;
        }
    }

    for (uint16_t j = 0; j < LED_MATRIX_H * LED_MATRIX_W; j++)
    {
        buffer[j] = next_generation[j];
    }
}