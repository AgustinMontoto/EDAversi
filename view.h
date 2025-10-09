/**
 * @brief Implements the Reversi game view
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#ifndef VIEW_H
#define VIEW_H

#include "model.h"

/**
 * @brief Initializes a game view.
 */
void initView();

/**
 * @brief Frees the game view.
 */
void freeView();

/**
 * @brief Draws the posible moves in the players turn.
 *
 * @param model The game model.
 */
void drawPosibleMoves(GameModel &model, Moves validMoves);
/**
 * @brief Draws the game view.
 *
 * @param model The game model.
 */
void drawView(GameModel &model, Moves validMoves);

/**
 * @brief Returns the square over the mouse pointer.
 *
 * @return The square.
 */
Square getSquareOnMousePointer();

/**
 * @brief Indicates whether the mouse pointer is over the "Play black" button.
 *
 * @return true or false.
 */
bool isMousePointerOverPlayBlackButton();

/**
 * @brief Indicates whether the mouse pointer is over the "Play white" button.
 *
 * @return true or false.
 */
bool isMousePointerOverPlayWhiteButton();

#endif
