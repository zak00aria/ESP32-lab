#include "multiplexed_leds_matrix.h"

void multiplexed_LEDs_matrix_init(
    Multiplexed_LEDs_matrix_t *multiplexed_LEDs_matrix,
    uint8_t *LEDs_state,
    uint8_t width,
    uint8_t height,
    gpio_num_t *rows_control_pins_list,
    gpio_num_t *cols_control_pins_list)
{
    gpio_config_t pGPIOConfig = {};
    pGPIOConfig.mode = GPIO_MODE_OUTPUT;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.intr_type = GPIO_INTR_DISABLE;
    pGPIOConfig.pin_bit_mask = 0;

    for (uint8_t i = 0; i < width; i++)
    {
        pGPIOConfig.pin_bit_mask |= 1ULL << cols_control_pins_list[i];
    }
    for (uint8_t i = 0; i < height; i++)
    {
        pGPIOConfig.pin_bit_mask |= 1ULL << rows_control_pins_list[i];
    }
    gpio_config(&pGPIOConfig);

    for (uint8_t i = 0; i < height; i++)
    {
        gpio_set_level(rows_control_pins_list[i], 1);
    }

    multiplexed_LEDs_matrix->width = width;
    multiplexed_LEDs_matrix->height = height;
    multiplexed_LEDs_matrix->LEDs_state = LEDs_state;
    multiplexed_LEDs_matrix->rows_control_pins_list = rows_control_pins_list;
    multiplexed_LEDs_matrix->cols_control_pins_list = cols_control_pins_list;
}

void multiplexed_LEDs_matrix_refresh(Multiplexed_LEDs_matrix_t *LEDs_matrix)
{
    uint16_t LED_index = 0;
    for (int8_t x = 0; x < LEDs_matrix->width; x++)
    {
        LED_index = x;
        for (int8_t y = 0; y < LEDs_matrix->height; y++, LED_index += LEDs_matrix->width)
        {
            gpio_set_level(LEDs_matrix->rows_control_pins_list[y], LEDs_matrix->LEDs_state[LED_index]);
        }
        gpio_set_level(LEDs_matrix->cols_control_pins_list[x], 0);
        vTaskDelay(25 / portTICK_PERIOD_MS);
        gpio_set_level(LEDs_matrix->cols_control_pins_list[x], 1);
    }
}

void multiplexed_LEDs_matrix_set(
    Multiplexed_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y,
    Multiplexed_LEDs_matrix_level_t level)
{
    // get LED index
    uint16_t LED_index = 0;
    for (uint8_t i = 0; i < y; i++)
    {
        LED_index += LEDs_matrix->width;
    }
    LED_index += x;
    // set LED level
    LEDs_matrix->LEDs_state[LED_index] = level;
}

Multiplexed_LEDs_matrix_level_t multiplexed_LEDs_matrix_get(
    Multiplexed_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y)
{
    // get LED index
    uint16_t LED_index = 0;
    for (uint8_t i = 0; i < y; i++)
    {
        LED_index += LEDs_matrix->width;
    }
    LED_index += x;
    // get LED level by index
    return LEDs_matrix->LEDs_state[LED_index];
}

void multiplexed_LEDs_matrix_reset(Multiplexed_LEDs_matrix_t *LEDs_matrix)
{
    uint16_t LED_index = 0;
    for (uint8_t y = 0; y < LEDs_matrix->height; y++)
    {
        for (uint8_t x = 0; x < LEDs_matrix->width; x++, LED_index++)
        {
            LEDs_matrix->LEDs_state[LED_index] = MLM_LOW;
        }
    }
}
