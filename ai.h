/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#ifndef AI_H
#define AI_H

#include "model.h"

struct treeNode {
    Square square;
    tree_logic state;  // Para copiar el estado esencial, no GameModel completo
    std::vector<treeNode*> children;
    int value;
};

/**
 * @brief Gets the best move for the AI player.
 *
 * @param model The game model.
 * @return The best move.
 */
Square getBestMove(GameModel &model);

#endif