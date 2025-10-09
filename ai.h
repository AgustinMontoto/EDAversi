/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#ifndef AI_H
#define AI_H

#include "model.h"

struct treeNode{
    Square square;
    GameModel model;
    std::vector<treeNode *> children;
};

/**
 * @brief Returns the best move for a certain position.
 *
 * @return The best move.
 */
Square getBestMove(GameModel &model);

#endif
