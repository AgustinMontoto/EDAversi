/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <cstdlib>
#include <cstdio>
#include <queue>

#include "ai.h"
#include "controller.h"

typedef std::vector<treeNode*> Tree_level_t;

int value_state(tree_logic const& state, Player ia_player, Player humanPlayer){
    
    int score_dif = getScore(state, ia_player) - getScore(state, humanPlayer);

    if(score_dif > 0){
        return 1;       //ia wins
    }
    else if(score_dif < 0){
        return -1;      //human wins
    }
    else{
        return 0;       //empate
    }
}

void init_tree(tree_logic const& state, Tree_level_t& Tree_level, int depth, int maxDepth, int &g_nodesExplored, int g_maxNodes, bool fuerza_bruta){

    if (g_nodesExplored >= g_maxNodes) {
        return;
    }
    
    if ((depth >= maxDepth && !fuerza_bruta) || state.gameOver) {
        return;
    }
    
    Moves valid_moves;
    getValidMoves(state, valid_moves);     //hijos del nodo que evaluo
    
    for(auto move : valid_moves){

        if (g_nodesExplored >= g_maxNodes) {
            return;
        }
        
        treeNode* node = new treeNode;
        node->square = move;
        node->value = 0;
        
        g_nodesExplored++;  // Incrementar al crear el nodo
        
        tree_logic child_state = state;
        playMove(child_state, move);        //modifica child_state (se fija game_over y current_player)
        node->state = child_state;

        Tree_level.push_back(node);

        init_tree(child_state, node->children, depth+1, maxDepth, g_nodesExplored, g_maxNodes, fuerza_bruta);
    }
}

void free_tree(Tree_level_t & Tree_level){
    for(auto level : Tree_level){
        free_tree(level->children);
        delete level;
    }
    Tree_level.clear();
}

int Recorrido_BFS(Tree_level_t& Tree_level, int maxNodesExplored){     //uso el algoritmo BFS para la parte tres del juego
    static int maxValue = -1000;       //valor random muy chico para el caso del primer hijo   
    static int minValue = 1000;
    int g_nodesExplored = 0;

    if(Tree_level.empty()){
        return;
    }
    std::queue<treeNode *> q;
    for(auto node:Tree_level){
        q.push(node);
    }
    while(!q.empty() && g_nodesExplored < maxNodesExplored && maxValue < minValue){
        treeNode *node = q.front();
        q.pop();

        //visit node
        if (node->model.currentPlayer == ia_player) {     // Turno de la IA: maximizar    
            int value = value_state(node->model);
            maxValue += value;
        }
        else {                // Turno del humano: minimizar
            int value = value_state(node->model);
            minValue -= value;
        }

        g_nodesExplored++;

        //funcion para procesar el nodo segun juegos posibles
        for(auto childNode : node->children){
            q.push(childNode);
        }
    }
    Tree_level->value = (maxValue +1000) - (minValue-1000);

    return Tree_level->value;  // retorno los puntos de cada jugada 

}

// Algoritmo Minimax recursivo
int minimax(treeNode* node, Player ia_player, Player humanPlayer, int depth, int maxDepth, bool fuerza_bruta) {
    
    // Caso base: nodo hoja o profundidad máxima
    if ((depth >= maxDepth && !fuerza_bruta) || node->children.empty()) {
        return value_state(node->state, ia_player, humanPlayer);
    }
    
    //evaluo caso inductivo
    if (node->state.currentPlayer == ia_player) {     // Turno de la IA: maximizar    
        int maxValue = -1000;       //valor muy pequeño para comparar
        
        for (auto child : node->children) {     //se fija en todos sus nodos hijos a ver cual tiene el mayor valor y lo guarda
            int value = minimax(child, ia_player, humanPlayer, depth + 1, maxDepth, fuerza_bruta);
            maxValue = std::max(maxValue, value);
        }
        
        node->value = maxValue;
        return maxValue;
        
    } else {                // Turno del humano: minimizar
        int minValue = 1000;        //valor muy grande para comparar
        
        for (auto child : node->children) {
            int value = minimax(child, ia_player, humanPlayer, depth + 1, maxDepth, fuerza_bruta);
            minValue = std::min(minValue, value);
        }
        
        node->value = minValue;
        return minValue;
    }
}

// Obtiene el mejor movimiento usando Minimax
Square getBestMove(GameModel &model) {

    Player ia_player = (model.humanPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;

    //Poda stats
    int maxDepth = 5;       
    int g_maxNodes = 100000;  
    int g_nodesExplored = 0;
    bool fuerza_bruta = false;
    
    int empty_places = BOARD_SIZE * BOARD_SIZE - (getScore(model, ia_player) + getScore(model, model.humanPlayer));
    
    if(empty_places <= 8){      //activar fuerza bruta
        fuerza_bruta = true;
        printf("Modo fuerza bruta activado. Casillas vacías: %d\n", empty_places);
    } else if(empty_places <= 14) {
        maxDepth = 7;  // Mayor profundidad cuando quedan pocas casillas
    }

    tree_logic current_state = gameStateFromModel(model);
    
    std::vector<treeNode*> gameTree;
    init_tree(current_state, gameTree, 0, maxDepth, g_nodesExplored, g_maxNodes, fuerza_bruta);

    if (gameTree.empty()) {
        return GAME_INVALID_SQUARE;
    }

    int bestValue = -1000;
    Square bestMove = GAME_INVALID_SQUARE;
    
    for (auto node : gameTree) {
        int value = minimax(node, ia_player, model.humanPlayer, 0, maxDepth, fuerza_bruta);
        
        // Quedarse con el movimiento de mayor valor
        if (value > bestValue) {
            bestValue = value;
            bestMove = node->square;
        }
    }
    
    printf("Nodos explorados: %d, Mejor valor: %d, Casillas vacías: %d\n", g_nodesExplored, bestValue, empty_places);
    free_tree(gameTree);
    return bestMove;
}