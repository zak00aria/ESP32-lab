#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H 1

#include <stdint.h>
#include "multiplexed_leds_matrix.h"

typedef enum {
    STATE_DEAD = 0,
    STATE_ALIVE,
    STATE_WILL_DIE, // alive -> dead next generation
    STATE_BIRTH // dead -> alive next generation
} Game_of_life_states_t;

void advance_game_of_life(Multiplexed_LEDs_matrix_t *LEDs_matrix);
uint8_t count_alive_neighbors(Multiplexed_LEDs_matrix_t *LEDs_matrix, uint8_t x, uint8_t y);

#endif
