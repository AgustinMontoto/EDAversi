/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <cstdlib>

#include <queue>
#include "ai.h"
#include "controller.h"

typedef std::vector<treeNode*> Tree_level_t;

int value_state(GameModel & model){
    if(model.gameOver) return 0;
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

void init_tree(GameModel const& model, Tree_level_t& Tree_level){
    if (/*depth >= maxDepth ||*/ model.gameOver) {
        return;
    }
    Moves valid_moves;

    getValidMoves(model, valid_moves);     //hijos del nodo que evaluo
    int i=0;
    for(auto move:valid_moves){
        treeNode* node = new treeNode;
        node->square= move;
        
        GameModel child_model = model;
        playMove(child_model, move);        //modifica child_model (se fija game_over y current_player)
        node->model = child_model;

        Tree_level.push_back(node);

        init_tree(child_model, node->children);
    }

}
void free_tree(Tree_level_t & Tree_level){
    for(auto level: Tree_level){
        free_tree(level->children);
        delete level;
    }
    Tree_level.clear();
}

void Recorrido_BFS(Tree_level_t& Tree_level){     //uso el algoritmo BFS para la parte tres del juego
    if(Tree_level.empty()){
        return;
    }
    std::queue<treeNode *> q;
    for(auto node:Tree_level){
        q.push(node);
    }
    while(!q.empty()){
        treeNode *node = q.front();
        q.pop();
        //funcion para procesar el nodo segun juegos posibles
        for(auto childNode : node->children){
            q.push(childNode);
        }
    }
}

// Algoritmo Minimax recursivo
int minimax(treeNode* node, Player ia_player) {
    // Caso base: nodo hoja
    if (node->children.empty()) {
        return value_state(node->model);
    }
    
    //evaluo caso inductivo
    if (node->model.currentPlayer == ia_player) {     // Turno de la IA: maximizar
        int maxValue = -1000;       //valor random muy chico para el caso del primer hijo       
        
        for (auto child : node->children) {     //se fija en todos sus nodos hijos a ver cual tiene el mayor valor y lo guarda
            int value = minimax(child, ia_player);
            maxValue = std::max(maxValue, value);
        }
        
        node->value = maxValue;
        return maxValue;
        
    } else {                // Turno del humano: minimizar
        int minValue = 1000;
        
        for (auto child : node->children) {
            int value = minimax(child, ia_player);
            minValue = std::min(minValue, value);
        }
        
        node->value = minValue;
        return minValue;
    }
}

// Obtiene el mejor movimiento usando Minimax-Parte 2
Square getBestMove(GameModel &model) {

    std::vector<treeNode*> gameTree;
    init_tree(model, gameTree);
    
    if (gameTree.empty()) {
        return GAME_INVALID_SQUARE;
    }

    Player ia_player = (model.humanPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
    int bestValue = -1000;
    Square bestMove;
    
    for (auto node : gameTree) {
        int value = minimax(node, ia_player);
        
        // Quedarse con el movimiento de mayor valor
        if (value > bestValue) {
            bestValue = value;
            bestMove = node->square;
        }
    }

    free_tree(gameTree);
    return bestMove;

    //Returns a random valid move...
    /*Moves validMoves;
    getValidMoves(model, validMoves);

    int index = rand() % validMoves.size();
    return validMoves[index]; */
}