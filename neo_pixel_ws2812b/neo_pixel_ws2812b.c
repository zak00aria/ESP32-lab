#include "neo_pixel_ws2812b.h"

rmt_symbol_word_t symbol_0 = {
    .level0 = 1,
    .duration0 = 40,
    .level1 = 0,
    .duration1 = 85};

rmt_symbol_word_t symbol_1 = {
    .level0 = 1,
    .duration0 = 80,
    .level1 = 0,
    .duration1 = 45};

size_t encoder_callback(
    const void *data,
    size_t data_size,
    size_t symbols_written,
    size_t symbols_free,
    rmt_symbol_word_t *symbols,
    bool *done,
    void *arg);

void neo_pixel_init(
    neo_pixel_t *neo_pixel,
    gpio_num_t gpio_num,
    size_t *encoder_current_pos,
    uint8_t *pixels,
    uint8_t width,
    uint8_t height)
{
    rmt_tx_channel_config_t tx_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .gpio_num = gpio_num,
        .mem_block_symbols = 64,
        .trans_queue_depth = 4,
        .resolution_hz = 100 * 1000 * 1000, // 100 MHz = 10 ns per tick
    };

    rmt_new_tx_channel(&tx_config, &(neo_pixel->rmt_channel_handle));
    rmt_enable(neo_pixel->rmt_channel_handle);

    // Configure encoder
    rmt_simple_encoder_config_t encoder_config = {
        .callback = encoder_callback,
        .arg = encoder_current_pos,
    };
    rmt_new_simple_encoder(&encoder_config, &(neo_pixel->encoder));

    neo_pixel->pixels = pixels;
    neo_pixel->pixels_length = width * height * 3;
    neo_pixel->width = width;
    neo_pixel->height = height;
    neo_pixel->rmt_transmit_config = (rmt_transmit_config_t){
        .loop_count = 0,
    };
}

void neo_pixel_refresh(
    neo_pixel_t *neo_pixel)
{
    rmt_transmit(
        neo_pixel->rmt_channel_handle,
        neo_pixel->encoder,
        neo_pixel->pixels,
        neo_pixel->pixels_length,
        &(neo_pixel->rmt_transmit_config));
    rmt_tx_wait_all_done(neo_pixel->rmt_channel_handle, portMAX_DELAY);
}

void neo_pixel_fill(
    neo_pixel_t *neo_pixel,
    const uint8_t color_rgb[static 3])
{
    for (uint16_t pixel_index = 0; pixel_index < neo_pixel->width * neo_pixel->height; pixel_index++)
    {
        neo_pixel_set_pixel_by_index(
            neo_pixel,
            pixel_index,
            color_rgb);
    }
}

void neo_pixel_clear(
    neo_pixel_t *neo_pixel)
{
    neo_pixel_fill(neo_pixel, NEO_PIXEL_COLOR_RGB(0, 0, 0));
}

void neo_pixel_set_pixel_by_index(
    neo_pixel_t *neo_pixel,
    uint16_t pixel_index,
    const uint8_t color_rgb[static 3])
{
    pixel_index += pixel_index << 1; // this is same as (pixel_index*3)
    if (pixel_index >= neo_pixel->pixels_length)
        return;
    neo_pixel->pixels[pixel_index + 0] = color_rgb[1]; // green
    neo_pixel->pixels[pixel_index + 1] = color_rgb[0]; // red
    neo_pixel->pixels[pixel_index + 2] = color_rgb[2]; // blue
}

void neo_pixel_get_pixel_by_index(
    const neo_pixel_t *neo_pixel,
    uint16_t pixel_index,
    uint8_t color_rgb[static 3])
{
    pixel_index += pixel_index << 1; // this is same as (pixel_index*3)
    if (pixel_index >= neo_pixel->pixels_length)
        return;
    color_rgb[1] = neo_pixel->pixels[pixel_index + 0]; // green
    color_rgb[0] = neo_pixel->pixels[pixel_index + 1]; // red
    color_rgb[2] = neo_pixel->pixels[pixel_index + 2]; // blue
}

void neo_pixel_set_pixel(
    neo_pixel_t *neo_pixel,
    uint8_t x,
    uint8_t y,
    const uint8_t color_rgb[static 3])
{
    if (x >= neo_pixel->width || y >= neo_pixel->height)
        return;
    uint16_t pixel_index = x;
    for (uint8_t temp_y = 0; temp_y < y; temp_y++)
        pixel_index += neo_pixel->width;
    neo_pixel_set_pixel_by_index(neo_pixel, pixel_index, color_rgb);
}

void neo_pixel_get_pixel(
    const neo_pixel_t *neo_pixel,
    uint8_t x,
    uint8_t y,
    uint8_t color_rgb[static 3])
{
    if (x >= neo_pixel->width || y >= neo_pixel->height)
        return;
    uint16_t pixel_index = x;
    for (uint8_t temp_y = 0; temp_y < y; temp_y++)
        pixel_index += neo_pixel->width;
    neo_pixel_get_pixel_by_index(neo_pixel, pixel_index, color_rgb);
}

size_t encoder_callback(
    const void *data,
    size_t data_size,
    size_t symbols_written,
    size_t symbols_free,
    rmt_symbol_word_t *symbols,
    bool *done,
    void *arg)
{
    if (symbols_free < 8)
    {
        return 0;
    }
    uint8_t *data_bytes = (uint8_t *)data;
    size_t *pos = (size_t *)arg;

    if (symbols_written == 0)
    {
        *pos = 0;
    }
    if (*pos < data_size)
    {
        for (int offset = 0; offset < 8; offset++)
        {
            if ((data_bytes[*pos] >> offset) & 1)
            {
                symbols[offset].level0 = symbol_1.level0;
                symbols[offset].duration0 = symbol_1.duration0;
                symbols[offset].level1 = symbol_1.level1;
                symbols[offset].duration1 = symbol_1.duration1;
            }
            else
            {
                symbols[offset].level0 = symbol_0.level0;
                symbols[offset].duration0 = symbol_0.duration0;
                symbols[offset].level1 = symbol_0.level1;
                symbols[offset].duration1 = symbol_0.duration1;
            }
        }
        (*pos)++;
        return 8;
    }
    symbols[0].level0 = 0;
    symbols[0].duration0 = 250;
    symbols[0].level1 = 0;
    symbols[0].duration1 = 250;
    *done = 1;
    return 1;
}
