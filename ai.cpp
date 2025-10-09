/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <cstdlib>

#include "ai.h"
#include "controller.h"

int value_state(GameModel & model){
    Player ia_player =
            (model.humanPlayer == PLAYER_WHITE)
                ? PLAYER_BLACK
                : PLAYER_WHITE;
    int score_dif=getScore(model,ia_player)-getScore(model, model.humanPlayer);
    if(score_dif>0){
        return 1;       //ia wins
    }
    else if(score_dif<0){
        return -1;      //human wins
    }
    else{
        return 0;       //empate
    }
}

void init_tree(GameModel const& model, std::vector<treeNode*>& Tree_level){
    if (/*depth >= maxDepth ||*/ model.gameOver) {
        return;
    }
    Moves valid_moves;
    GameModel model_copy = model;
    getValidMoves(model_copy, valid_moves);
    int i=0;
    for(auto move:valid_moves){
        treeNode* node = new treeNode;
        node->square= move;
        node->model = model_copy;
        Tree_level.push_back(node);
        playMove(model_copy, Tree_level[i]->square);

        init_tree(model, Tree_level[i]->children);
        i++;
    }

}
void free_tree(std::vector<treeNode*>& Tree_level){
    for(auto level: Tree_level){
        free_tree(level->children);
        delete level;
    }
    Tree_level.clear();
}

Square getBestMove(GameModel &model)
{
    // To-do: your code goes here...
    std::vector<treeNode*> gameTree;
    init_tree(model, gameTree);

    free_tree(gameTree);
    // +++ TEST
    //Returns a random valid move...
    /*Moves validMoves;
    getValidMoves(model, validMoves);

    int index = rand() % validMoves.size();
    return validMoves[index];*/ 
    // --- TEST
}
