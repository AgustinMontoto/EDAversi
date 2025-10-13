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
#include <cstring>

#include "ai.h"
#include "controller.h"
#include "view.h"

typedef std::vector<treeNode*> Tree_level_t;

// Contador global para controlar las llamadas a drawView()
static int g_drawViewCounter = 0;
static const int DRAW_VIEW_INTERVAL = 1000; // Llamar cada 1000 nodos

// Variable global para guardar referencia al modelo (necesaria para drawView)
static GameModel* g_currentModel = nullptr;

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
static const std::vector<std::pair<std::string,std::string>> OPENING_BOOK = {
    {"C4C3", "D3"},     // Diagonal → D3
    {"C4E3", "F5"},     // Perpendicular → F5
    {"C4C5", "D6"},     // Paralela → D6
    {"C4E3F4C5", "D6"}, // Mimura/Shaman rama base
    {"C4C3D3C5", "D6"}, // Landau/Buffalo rama base
};

// Busca coincidencia exacta de prefijo y valida que la jugada sea legal.
static bool openingBookBestMove(const GameModel& model, Square& outMove) {
    std::string hist = historyStringUpper(model);
    for (const auto& kv : OPENING_BOOK) {
        const std::string& prefix = kv.first;
        if (hist == prefix) {
            Square candidate = fromAlg(kv.second);
            if (!isSquareValid(candidate)) return false;
            Moves legal;
            getValidMoves(model, legal);
            for (auto m : legal) {
                if (m.x == candidate.x && m.y == candidate.y) {
                    outMove = candidate;
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}

int pieceDifference(tree_logic & model, Player ia_player){
    Player humanPlayer = (ia_player == PLAYER_WHITE) ? 
                            PLAYER_BLACK : PLAYER_WHITE;
    int score_dif=getScore(model,ia_player)-getScore(model, humanPlayer);
    return score_dif;
}

int evaluateMovility(tree_logic & model, Player ia_player){
    Moves ia_moves;
    Moves human_moves;
    getValidMoves(model, ia_moves);
    getValidMoves(model, human_moves);
    return ia_moves.size() - human_moves.size();
}

int evaluateAdyacents(tree_logic &model, Player ia_player, Square move){
    if(move.x == 1 && move.y == 0 || move.x == 0 && move.y == 1 || move.x == 1 && move.y == 1||
       move.x == BOARD_SIZE-1 && move.y == 0 || move.x == BOARD_SIZE-1 && move.y == 1 || move.x == 0 && move.y == 1 ||
        move.x == 0 && move.y == BOARD_SIZE-1 || move.x == 1 && move.y == BOARD_SIZE-1 || move.x == 1 && move.y == BOARD_SIZE||
        move.x == BOARD_SIZE-1 && move.y == BOARD_SIZE-1 || move.x == BOARD_SIZE-1 && move.y == BOARD_SIZE || move.x == BOARD_SIZE && move.y == BOARD_SIZE-1){
           return -1;   //penalizo adyacentes a las esquinas
    }
    return 0; 
}

int evaluateStability(tree_logic &model, Player ia_player, Square move){
    int stability = -1;
    Piece ia_piece = (ia_player == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;

    static int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };

    for(int i = 0; i<8; i++){
        int dir[2] = {directions[i][0], directions[i][1]};

        for(int j = 1; j < BOARD_SIZE && stability; j++){
            Square currentPos = {move.x + j*dir[0], move.y + j*dir[1]};
            
            if(isSquareValid(currentPos) == false){
                stability = false;
                break;
            }

            if(getBoardPiece(model, currentPos) == PIECE_EMPTY){
                stability = false;
                break;
            }else if(getBoardPiece(model, currentPos) == ia_piece){
                stability = true;
                continue;
            }else{
                stability = false;
                break;
            }    
        }

        if(stability == true){
            break;
        }
    }

    return stability == true? 1 : 0;
}

int isXsquareCorner(tree_logic &model, Square move){
    if((move.x == 0 && move.y == 0) || (move.x == 0 && move.y == BOARD_SIZE-1) || 
       (move.x == BOARD_SIZE-1 && move.y == 0) || (move.x == BOARD_SIZE-1 && move.y == BOARD_SIZE-1)){
        return 1;
    }
    return 0;
}

int value_state(tree_logic & model, Player ia_player, Square move){
    if(model.gameOver) return 0; 
    
    int score_dif = pieceDifference(model, ia_player);
    int movility = evaluateMovility(model, ia_player);
    int adyacents = evaluateAdyacents(model, ia_player, move);
    int corner = isXsquareCorner(model, move);

    int value = (10 * score_dif) + (5 * movility) + 
                (50 * corner) + (3 * adyacents);

    return value;
}

void init_tree(tree_logic const& state, Tree_level_t& Tree_level, int depth, int maxDepth, 
               int &g_nodesExplored, int g_maxNodes, bool fuerza_bruta){

    if (g_nodesExplored >= g_maxNodes) {
        return;
    }
    
    if ((depth >= maxDepth && !fuerza_bruta) || state.gameOver) {
        return;
    }
    
    Moves valid_moves;
    getValidMoves(state, valid_moves);
    
    for(auto move : valid_moves){
        if (g_nodesExplored >= g_maxNodes) {
            return;
        }
        
        treeNode* node = new treeNode;
        node->square = move;
        node->value = 0;
        
        g_nodesExplored++;
        
        // CRÍTICO: Llamar a drawView() periódicamente
        g_drawViewCounter++;
        if (g_drawViewCounter >= DRAW_VIEW_INTERVAL && g_currentModel != nullptr) {
            Moves emptyMoves; // No mostramos movimientos válidos durante el cálculo
            drawView(*g_currentModel, emptyMoves);
            g_drawViewCounter = 0;
        }
        
        tree_logic child_state = state;
        playMove(child_state, move);
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

int Recorrido_BFS(treeNode* node, Player ia_player, int maxNodesExplored){
    static int maxValue = -100;
    static int minValue = 100;
    int g_nodesExplored = 0;
    Square move = node->square;

    if(node->children.empty()){
        return 0;
    }
    std::queue<treeNode *> q;
    for(auto treeNode:node->children){
        q.push(node);
    }
    while(!q.empty() && g_nodesExplored < maxNodesExplored && maxValue < minValue){
        treeNode *node = q.front();
        q.pop();

        // CRÍTICO: Llamar a drawView() periódicamente
        g_drawViewCounter++;
        if (g_drawViewCounter >= DRAW_VIEW_INTERVAL && g_currentModel != nullptr) {
            Moves emptyMoves;
            drawView(*g_currentModel, emptyMoves);
            g_drawViewCounter = 0;
        }

        if (node->state.currentPlayer == ia_player) {
            int value = value_state(node->state, ia_player, move);
            maxValue += value;
        }
        else {
            int value = value_state(node->state, ia_player, move);
            minValue -= value;
        }

        g_nodesExplored++;

        for(auto childNode : node->children){
            q.push(childNode);
        }
    }

    return (maxValue +1000) - (minValue-1000);
}

// Algoritmo Minimax recursivo
int minimax(treeNode* node, Player ia_player, Player humanPlayer, int depth, int maxDepth, bool fuerza_bruta) {
    
    // CRÍTICO: Llamar a drawView() periódicamente
    g_drawViewCounter++;
    if (g_drawViewCounter >= DRAW_VIEW_INTERVAL && g_currentModel != nullptr) {
        Moves emptyMoves;
        drawView(*g_currentModel, emptyMoves);
        g_drawViewCounter = 0;
    }
    
    // Caso base
    if ((depth >= maxDepth && !fuerza_bruta) || node->children.empty()) {
        return value_state(node->state, ia_player, node->square);
    }
    
    if (node->state.currentPlayer == ia_player) {
        int maxValue = -1000;
        
        for (auto child : node->children) {
            int value = minimax(child, ia_player, humanPlayer, depth + 1, maxDepth, fuerza_bruta);
            maxValue = std::max(maxValue, value);
        }
        
        node->value = maxValue;
        return maxValue;
        
    } else {
        int minValue = 1000;
        
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

    // Guardar referencia al modelo para las llamadas a drawView()
    g_currentModel = &model;
    
    // Resetear el contador al inicio
    g_drawViewCounter = 0;

    // 1) Intentar jugar de libro de aperturas
    Square bookMove;
    if (openingBookBestMove(model, bookMove)) {
        g_currentModel = nullptr; // Limpiar la referencia
        return bookMove;
    }
    
    Player ia_player = (model.humanPlayer == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;

    int maxDepth = 5;       
    int g_maxNodes = 50000;  
    int g_nodesExplored = 0;
    bool fuerza_bruta = false;
    
    int empty_places = BOARD_SIZE * BOARD_SIZE - (getScore(model, ia_player) + getScore(model, model.humanPlayer));
    
    if(empty_places <= 8){
        fuerza_bruta = true;
        printf("Modo fuerza bruta activado. Casillas vacías: %d\n", empty_places);
    } else if(empty_places <= 14) {
        maxDepth = 7;
    }

    tree_logic current_state = gameStateFromModel(model);
    
    std::vector<treeNode*> gameTree;
    init_tree(current_state, gameTree, 0, maxDepth, g_nodesExplored, g_maxNodes, fuerza_bruta);

    if (gameTree.empty()) {
        g_currentModel = nullptr; // Limpiar la referencia
        return GAME_INVALID_SQUARE;
    }

    int bestValue = -1000;
    Square bestMove = GAME_INVALID_SQUARE;
    
    for (auto node : gameTree) {
        int value = minimax(node, ia_player, model.humanPlayer, 0, maxDepth, fuerza_bruta);
        
        if (value > bestValue) {
            bestValue = value;
            bestMove = node->square;
        }
    }
    
    printf("Nodos explorados: %d, Mejor valor: %d, Casillas vacías: %d\n", g_nodesExplored, bestValue, empty_places);
    free_tree(gameTree);
    
    // Calcular movimientos válidos y llamada final para actualizar la UI
    Moves validMoves;
    getValidMoves(model, validMoves);
    drawView(model, validMoves);
    
    // Limpiar la referencia al modelo
    g_currentModel = nullptr;
    
    return bestMove;
}