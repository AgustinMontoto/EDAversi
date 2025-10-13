/**
 * @brief Implements the Reversi game AI
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <cstdlib>
#include <cstdio>
#include <queue>
#include <unordered_map>
#include <string>
#include <cctype>
#include "ai.h"
#include "controller.h"

typedef std::vector<treeNode*> Tree_level_t;

// Convierte Square {x,y} a notación Othello "C4" (A1 es (0,0), H8 es (7,7))
static std::string toAlg(Square s) {
    char col = char('A' + s.x);
    char row = char('1' + s.y);
    return std::string{col, row};
}

// Parseo simple "C4" -> Square {2,3}. Retorna GAME_INVALID_SQUARE si hay error.
static Square fromAlg(const std::string& alg) {
    if (alg.size() != 2) return GAME_INVALID_SQUARE;
    char c = std::toupper(alg[0]);
    char r = alg[1];
    if (c < 'A' || c > 'H' || r < '1' || r > '8') return GAME_INVALID_SQUARE;
    return Square{ int(c - 'A'), int(r - '1') };
}

// Devuelve la cadena de historial en mayúsculas "C4C3D3..."
static std::string historyStringUpper(const GameModel& model) {
    std::string s;
    s.reserve(model.moveHistory.size()*2);
    for (const auto& mv : model.moveHistory) {
        std::string a = toAlg(mv);
        s.push_back(std::toupper(a[0]));
        s.push_back(a[1]); // los dígitos ya están ok
    }
    return s;
}

// Tabla de aperturas: prefijo -> mejor siguiente jugada (ambas en mayúsculas).
// Tomadas de Samsoft: Diagonal (C4 C3 → D3), Perpendicular (C4 E3 → F5 / F6),
// Paralela (C4 C5 → D6: el sitio anota explícitamente que el libro de WZebra
// da +6 para negras tras D6), y algunas continuaciones frecuentes.
static const std::vector<std::pair<std::string,std::string>> OPENING_BOOK = {
    // Respuestas tempranas canónicas
    {"C4C3", "D3"},     // Diagonal → D3
    {"C4E3", "F5"},     // Perpendicular → F5 (también existe F6)
    {"C4C5", "D6"},     // Paralela → D6 (recomendado por Samsoft/WZebra)

    // Un par de continuaciones típicas
    {"C4E3F4C5", "D6"}, // Mimura/Shaman rama base: ...F4 c5 → D6
    {"C4C3D3C5", "D6"}, // Landau/Buffalo rama base: ...D3 c5 → D6
};

// Busca coincidencia exacta de prefijo y valida que la jugada sea legal.
// Si encuentra match, escribe en outMove y devuelve true.
static bool openingBookBestMove(const GameModel& model, Square& outMove) {
    std::string hist = historyStringUpper(model);
    for (const auto& kv : OPENING_BOOK) {
        const std::string& prefix = kv.first;
        if (hist == prefix) {
            Square candidate = fromAlg(kv.second);
            if (!isSquareValid(candidate)) return false;
            // Solo jugamos si también es legal ahora mismo
            Moves legal;
            getValidMoves(model, legal);
            for (auto m : legal) {
                if (m.x == candidate.x && m.y == candidate.y) {
                    outMove = candidate;
                    return true;
                }
            }
            // Si el libro sugiere algo ilegal (por desviación previa), no lo usamos
            return false;
        }
    }
    return false;
}

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

void Recorrido_BFS(treeNode* TreeNode, Player ia_player, Player humanPlayer,int maxNodesExplored){     //uso el algoritmo BFS para la parte tres del juego
    static int maxValue = -1000;       //valor random muy chico para el caso del primer hijo   
    static int minValue = 1000;
    int g_nodesExplored = 0;

    /*if(Tree_level.empty()){
        return;
    }*/
    std::queue<treeNode* > q;
    
    q.push(TreeNode);
    while(!q.empty() && g_nodesExplored < maxNodesExplored && maxValue < minValue){
        treeNode *node = q.front();
        q.pop();

        //visit node
        if (node->state.currentPlayer == ia_player) {     // Turno de la IA: maximizar    
            int value = value_state(node->state, ia_player, humanPlayer);
            maxValue += value;
        }
        else {                // Turno del humano: minimizar
            int value = value_state(node->state, ia_player, humanPlayer);
            minValue -= value;
        }

        g_nodesExplored++;

        //funcion para procesar el nodo segun juegos posibles
        for(auto childNode : node->children){
            q.push(childNode);
        }
    }
    TreeNode->value = (maxValue +1000) - (minValue-1000);
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

    // 1) Intentar jugar de libro de aperturas (si hay match y la movida es legal)
    Square bookMove;
    if (openingBookBestMove(model, bookMove)) {
        return bookMove;
    }
    
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