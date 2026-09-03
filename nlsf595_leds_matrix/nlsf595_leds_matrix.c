#include "nlsf595_leds_matrix.h"

void nlsf595_LEDs_matrix_init(
    nlsf595_LEDs_matrix_t *LEDs_matrix,
    uint8_t *LEDs_state,
    uint8_t width,
    uint8_t height,
    gpio_num_t SI,
    gpio_num_t SCK,
    gpio_num_t RCK)
{
    gpio_config_t pGPIOConfig = {};

    // config SI,SCK,RCK GPIO as outputs
    pGPIOConfig.mode = GPIO_MODE_OUTPUT;
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pGPIOConfig.intr_type = GPIO_INTR_DISABLE;
    pGPIOConfig.pin_bit_mask = 0;
    pGPIOConfig.pin_bit_mask |= 1ULL << SI;
    pGPIOConfig.pin_bit_mask |= 1ULL << SCK;
    pGPIOConfig.pin_bit_mask |= 1ULL << RCK;

    gpio_config(&pGPIOConfig);

    LEDs_matrix->width = width;
    LEDs_matrix->height = height;
    LEDs_matrix->LEDs_state = LEDs_state;
    LEDs_matrix->SI = SI;
    LEDs_matrix->SCK = SCK;
    LEDs_matrix->RCK = RCK;
}

void nlsf595_LEDs_matrix_refresh(nlsf595_LEDs_matrix_t *LEDs_matrix)
{
    uint16_t row_start_index = 0;
    for (int8_t y = 0; y < LEDs_matrix->height; y++, row_start_index += LEDs_matrix->width)
    {
        nlsf595_send_data(LEDs_matrix->LEDs_state + row_start_index, LEDs_matrix->width, LEDs_matrix->SI, LEDs_matrix->SCK);
    }
    nlsf595_save_data(LEDs_matrix->RCK);
}

void nlsf595_LEDs_matrix_set(
    nlsf595_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y,
    nlsf595_LEDs_matrix_level_t level)
{
    if (x >= LEDs_matrix->width || y >= LEDs_matrix->height)
        return;
    // get LED index (LED_index= x + y * LEDs_matrix->width)
    uint16_t LED_index = x;
    // I used a loop instead of (y*LEDs_matrix->width)
    for (uint8_t i = 0; i < y; i++)
    {
        LED_index += LEDs_matrix->width;
    }
    // set LED level
    LEDs_matrix->LEDs_state[LED_index] = level;
}

nlsf595_LEDs_matrix_level_t nlsf595_LEDs_matrix_get(
    nlsf595_LEDs_matrix_t *LEDs_matrix,
    uint8_t x,
    uint8_t y)
{
    if (x >= LEDs_matrix->width || y >= LEDs_matrix->height)
        return MLM_LOW;
    // get LED index (LED_index= x + y * LEDs_matrix->width)
    uint16_t LED_index = x;
    // I used a loop instead of (y*LEDs_matrix->width)
    for (uint8_t i = 0; i < y; i++)
    {
        LED_index += LEDs_matrix->width;
    }
    // get LED level by index
    return LEDs_matrix->LEDs_state[LED_index];
}

void nlsf595_LEDs_matrix_clear(nlsf595_LEDs_matrix_t *LEDs_matrix)
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

void nlsf595_send_data(const uint8_t *data, const uint8_t data_length, gpio_num_t SI, gpio_num_t SCK)
{
    for (uint8_t i = 0; i < data_length; i++)
    {
        gpio_set_level(SI, !data[i]);
        gpio_set_level(SCK, 1);
        gpio_set_level(SCK, 0);
    }
}

void nlsf595_save_data(gpio_num_t RCK)
{
    gpio_set_level(RCK, 0);
    gpio_set_level(RCK, 1);
    gpio_set_level(RCK, 0);
}
