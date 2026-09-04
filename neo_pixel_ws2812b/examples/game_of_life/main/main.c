#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/rmt_tx.h"
#include "neo_pixel_ws2812b.h"
#include "game_of_life.h"

#define GPIO_PIN 18
#define MATRIX_WIDTH 24
#define MATRIX_HEIGHT 24
#define INIT_CELLS_STATE_LENGTH 15

uint8_t pixels[MATRIX_WIDTH * MATRIX_HEIGHT * 3] = {0};
neo_pixel_t neo_pixel = {};

Game_of_life_states_t game_of_life_cells[MATRIX_WIDTH * MATRIX_HEIGHT] = {STATE_DEAD};
uint16_t game_of_life_init_cells_state[INIT_CELLS_STATE_LENGTH] = {126, 127, 149, 150, 174, 276, 277, 299, 300, 324, 402, 403, 425, 426, 450};
Game_of_life_t game_of_life = {};

static void refresh_LEDs_matrix_task(void *arg);
static void advance_game_of_life_task(void *arg);

void app_main()
{
  size_t encoder_current_pos;

  neo_pixel_init(
      &neo_pixel,
      GPIO_PIN,
      &encoder_current_pos,
      pixels,
      MATRIX_WIDTH,
      MATRIX_HEIGHT);

  game_of_life_init(
      &game_of_life,
      game_of_life_cells,
      MATRIX_WIDTH,
      MATRIX_HEIGHT,
      game_of_life_init_cells_state,
      INIT_CELLS_STATE_LENGTH);

  xTaskCreate(refresh_LEDs_matrix_task, "refresh_LEDs_matrix_task", 1024, NULL, 1, NULL);
  xTaskCreate(advance_game_of_life_task, "advance_game_of_life_task", 1024, NULL, 2, NULL);

  return;
  while (1)
  {
    for (uint8_t y = 0; y < game_of_life.height; y++)
      for (uint8_t x = 0; x < game_of_life.width; x++)
      {
        neo_pixel_set_pixel(&neo_pixel, x, y, game_of_life_get_cell(&game_of_life, x, y) == STATE_DEAD ? NEO_PIXEL_COLOR_RGB(0, 0, 0) : NEO_PIXEL_COLOR_RGB(0xcc, 0xff, 0xee));
      }
    neo_pixel_refresh(&neo_pixel);
    game_of_life_advance(&game_of_life);
    vTaskDelay(150 / portTICK_PERIOD_MS);
  }
}

static void refresh_LEDs_matrix_task(void *arg)
{
  while (1)
  {
    // update LEDs matrix
    for (uint8_t y = 0; y < game_of_life.height; y++)
      for (uint8_t x = 0; x < game_of_life.width; x++)
      {
        neo_pixel_set_pixel(&neo_pixel, x, y, game_of_life_get_cell(&game_of_life, x, y) == STATE_DEAD ? NEO_PIXEL_COLOR_RGB(0, 0, 0) : NEO_PIXEL_COLOR_RGB(0xcc, 0xff, 0xee));
      }
    neo_pixel_refresh(&neo_pixel);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

static void advance_game_of_life_task(void *arg)
{
  while (1)
  {
    // advance game of life
    game_of_life_advance(&game_of_life);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
