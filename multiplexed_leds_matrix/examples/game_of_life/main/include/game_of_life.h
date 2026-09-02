/**
 * @file game_of_life.h
 * @brief Conway's Game of Life implementation
 * @details This module provides a complete implementation of Conway's Game of Life,
 *          a cellular automaton where cells evolve based on the number of living neighbors.
 */

#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H 1

#include <stdint.h>

/**
 * @enum Game_of_life_states_t
 * @brief Represents the state of a cell in the Game of Life
 * 
 * Each cell can exist in one of four states that track both the current state
 * and the planned state for the next generation.
 */
typedef enum
{
    STATE_DEAD = 0,        /**< Cell is dead and will remain dead */
    STATE_BIRTH,           /**< Cell is currently dead but will become alive in the next generation */
    STATE_ALIVE,           /**< Cell is alive and will remain alive */
    STATE_WILL_DIE,        /**< Cell is currently alive but will die in the next generation */
} Game_of_life_states_t;

/**
 * @struct Game_of_life_t
 * @brief Represents the entire Game of Life grid
 * 
 * This structure maintains the game state including the grid dimensions
 * and the current state of all cells.
 */
typedef struct
{
    uint8_t width;                      /**< Width of the game grid in cells */
    uint8_t height;                     /**< Height of the game grid in cells */
    Game_of_life_states_t *cells_state; /**< Pointer to array of cell states (size: width × height) */
} Game_of_life_t;

/**
 * @brief Initializes a Game of Life instance
 * 
 * @param[out] game_of_life            Pointer to the Game_of_life_t structure to initialize
 * @param[in]  cells_state             Pointer to pre-allocated array for cell states (must be at least width × height elements)
 * @param[in]  width                   Width of the game grid (in cells)
 * @param[in]  height                  Height of the game grid (in cells)
 * @param[in]  init_cells_state        Array of initial cell indices to set as alive
 * @param[in]  init_cells_state_length Number of elements in init_cells_state array
 * 
 * @details All cells are initialized to STATE_DEAD except those specified in init_cells_state.
 *          The init_cells_state array contains linear indices into the grid (row-major order).
 * 
 * @note The caller is responsible for allocating and managing the memory for cells_state.
 */
void game_of_life_init(Game_of_life_t *game_of_life,
                       Game_of_life_states_t *cells_state,
                       uint8_t width,
                       uint8_t height,
                       uint16_t *init_cells_state,
                       uint16_t init_cells_state_length);

/**
 * @brief Advances the game by one generation
 * 
 * @param[in,out] game_of_life Pointer to the Game_of_life_t structure to advance
 * 
 * @details Applies Conway's Game of Life rules:
 *          - A dead cell with exactly 3 live neighbors becomes alive
 *          - A live cell with 2 or 3 live neighbors survives
 *          - All other cells die or stay dead
 *          
 *          This function updates all cell states from their current generation
 *          to the next generation based on neighbor counts.
 */
void game_of_life_advance(Game_of_life_t *game_of_life);

/**
 * @brief Counts the number of alive neighbors for a given cell
 * 
 * @param[in] game_of_life Pointer to the Game_of_life_t structure
 * @param[in] x            X coordinate of the cell (column)
 * @param[in] y            Y coordinate of the cell (row)
 * 
 * @return Number of alive neighbors (0-8)
 * 
 * @details Examines all 8 adjacent cells (Moore neighborhood) and counts
 *          those in STATE_ALIVE or STATE_WILL_DIE states (cells that are currently alive).
 *          
 * @note This function does not perform bounds checking; the caller must ensure
 *       that x and y are valid coordinates within the grid dimensions.
 */
uint8_t game_of_life_count_alive_neighbors(Game_of_life_t *game_of_life, uint8_t x, uint8_t y);

/**
 * @brief Retrieves the current state of a cell
 * 
 * @param[in] game_of_life Pointer to the Game_of_life_t structure
 * @param[in] x            X coordinate of the cell (column)
 * @param[in] y            Y coordinate of the cell (row)
 * 
 * @return The state of the cell (one of the Game_of_life_states_t values)
 * 
 * @note This function does not perform bounds checking; the caller must ensure
 *       that x and y are valid coordinates within the grid dimensions.
 */
Game_of_life_states_t game_of_life_get_cell(Game_of_life_t *game_of_life, uint8_t x, uint8_t y);

#endif
