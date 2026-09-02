#include "game_of_life.h"

void game_of_life_init(Game_of_life_t *game_of_life,
                       Game_of_life_states_t *cells_state,
                       uint8_t width,
                       uint8_t height,
                       uint16_t *init_cells_state,
                       uint16_t init_cells_state_length)
{
    game_of_life->width = width;
    game_of_life->height = height;
    game_of_life->cells_state = cells_state;
    for (uint16_t i = width * height - 1; i > 0; i--)
        cells_state[i] = STATE_DEAD;
    for (uint16_t i = 0; i < init_cells_state_length; i++)
        cells_state[init_cells_state[i]] = STATE_ALIVE;
}

uint8_t game_of_life_count_alive_neighbors(Game_of_life_t *game_of_life, uint8_t x, uint8_t y)
{
    uint8_t alive_neighbors_count = 0;
    uint8_t neighbors_state = STATE_DEAD;
    uint8_t left_neighbors_x = x > 0 ? x - 1 : game_of_life->width - 1;
    uint8_t right_neighbors_x = x < game_of_life->width - 1 ? x + 1 : 0;
    uint8_t top_neighbors_y = y > 0 ? y - 1 : game_of_life->height - 1;
    uint8_t bottom_neighbors_y = y < game_of_life->height - 1 ? y + 1 : 0;
    // nighbors coordinates (x,y)
    uint8_t nighbors[8][2] = {
        {left_neighbors_x, top_neighbors_y},     // left top nighbor
        {x, top_neighbors_y},                    // top nighbor
        {right_neighbors_x, top_neighbors_y},    // right top nighbor
        {left_neighbors_x, y},                   // left nighbor
        {right_neighbors_x, y},                  // right nighbor
        {left_neighbors_x, bottom_neighbors_y},  // left bottom nighbor
        {x, bottom_neighbors_y},                 // bottom nighbor
        {right_neighbors_x, bottom_neighbors_y}, // right bottom nighbor
    };
    for (uint8_t i = 0; i < 8; i++)
    {
        neighbors_state = game_of_life_get_cell(game_of_life, nighbors[i][0], nighbors[i][1]);
        alive_neighbors_count += (neighbors_state == STATE_ALIVE || neighbors_state == STATE_WILL_DIE) ? 1 : 0;
    }

    return alive_neighbors_count;
}

void game_of_life_advance(Game_of_life_t *game_of_life)
{
    uint16_t cell_index = 0;
    uint8_t cell_state = 0;
    uint8_t alive_neighbors_count = 0;

    for (uint8_t y = 0; y < game_of_life->height; y++)
    {
        for (uint8_t x = 0; x < game_of_life->width; x++)
        {
            alive_neighbors_count = game_of_life_count_alive_neighbors(game_of_life, x, y);
            cell_state = game_of_life->cells_state[cell_index];
            if (cell_state == STATE_ALIVE)
            {
                game_of_life->cells_state[cell_index] = (uint8_t)((alive_neighbors_count != 2 && alive_neighbors_count != 3) ? STATE_WILL_DIE : STATE_ALIVE);
            }
            else
            {
                game_of_life->cells_state[cell_index] = (uint8_t)(alive_neighbors_count == 3 ? STATE_BIRTH : STATE_DEAD);
            }
            cell_index++;
        }
    }

    for (uint16_t i = 0; i < game_of_life->height * game_of_life->width; i++)
    {
        cell_state = game_of_life->cells_state[i];
        if (cell_state == (uint8_t)STATE_WILL_DIE)
            game_of_life->cells_state[i] = (uint8_t)STATE_DEAD;
        else if (cell_state == (uint8_t)STATE_BIRTH)
            game_of_life->cells_state[i] = (uint8_t)STATE_ALIVE;
    }
}

Game_of_life_states_t game_of_life_get_cell(Game_of_life_t *game_of_life, uint8_t x, uint8_t y)
{
    if (x >= game_of_life->width || y >= game_of_life->height)
        return STATE_DEAD;
    uint16_t index = x;
    for (uint8_t temp_y = 0; temp_y < y; temp_y++)
        index += game_of_life->width;
    return game_of_life->cells_state[index];
}
