#include "game_of_life.h"

uint8_t count_alive_neighbors(Multiplexed_LEDs_matrix_t *LEDs_matrix, uint8_t x, uint8_t y)
{
    uint8_t alive_neighbors_count = 0;
    uint8_t neighbors_state = STATE_DEAD;
    // left nighbor
    if (x > 0)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x - 1, y);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    // left top nighbor
    if (x > 0 && y > 0)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x - 1, y - 1);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    // right nighbor
    if (x < LEDs_matrix->width - 1)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x + 1, y);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    // right top nighbor
    if (x < LEDs_matrix->width - 1 && y > 0)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x + 1, y - 1);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    // top nighbor
    if (y > 0)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x, y - 1);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    // bottom nighbor
    if (y < LEDs_matrix->height - 1)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x, y + 1);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }
    // left bottom nighbor
    if (x > 0 && y < LEDs_matrix->height - 1)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x - 1, y + 1);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    // right bottom nighbor
    if (x < LEDs_matrix->width - 1 && y < LEDs_matrix->height - 1)
    {
        neighbors_state = multiplexed_LEDs_matrix_get(LEDs_matrix, x + 1, y + 1);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    return alive_neighbors_count;
}

void advance_game_of_life(Multiplexed_LEDs_matrix_t *LEDs_matrix)
{
    uint16_t cell_index = 0;
    uint8_t cell_state = 0;
    uint8_t alive_neighbors_count = 0;

    for (uint8_t y = 0; y < LEDs_matrix->height; y++)
    {
        for (uint8_t x = 0; x < LEDs_matrix->width; x++)
        {
            alive_neighbors_count = count_alive_neighbors(LEDs_matrix, x, y);
            cell_state = LEDs_matrix->LEDs_state[cell_index];
            if (cell_state == STATE_ALIVE)
            {
                LEDs_matrix->LEDs_state[cell_index] = (uint8_t)((alive_neighbors_count != 2 && alive_neighbors_count != 3) ? STATE_WILL_DIE : STATE_ALIVE);
            }
            else
            {
                LEDs_matrix->LEDs_state[cell_index] = (uint8_t)(alive_neighbors_count == 3 ? STATE_BIRTH : STATE_DEAD);
            }
            cell_index++;
        }
    }

    for (uint16_t i = 0; i < LEDs_matrix->height * LEDs_matrix->width; i++)
    {
        cell_state = LEDs_matrix->LEDs_state[i];
        if (cell_state == (uint8_t)STATE_WILL_DIE)
            LEDs_matrix->LEDs_state[i] = (uint8_t)STATE_DEAD;
        else if (cell_state == (uint8_t)STATE_BIRTH)
            LEDs_matrix->LEDs_state[i] = (uint8_t)STATE_ALIVE;
    }
}
