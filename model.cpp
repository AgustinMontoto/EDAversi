/**
 * @brief Implements the Reversi game model
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <cstring>
#include <stdio.h>
#include <iostream>

#include "raylib.h"

#include "model.h"

void initModel(GameModel &model)
{
    model.tree.gameOver = true;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;

    memset(model.tree.board, PIECE_EMPTY, sizeof(model.tree.board));
}

void startModel(GameModel &model)
{
    model.tree.gameOver = false;
    model.first_human_try = true;

    model.tree.currentPlayer = PLAYER_BLACK;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;
    model.turnTimer = GetTime();

    memset(model.tree.board, PIECE_EMPTY, sizeof(model.tree.board));
    model.tree.board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2 - 1] = PIECE_WHITE;
    model.tree.board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2] = PIECE_BLACK;
    model.tree.board[BOARD_SIZE / 2][BOARD_SIZE / 2] = PIECE_WHITE;
    model.tree.board[BOARD_SIZE / 2][BOARD_SIZE / 2 - 1] = PIECE_BLACK;
    model.moveHistory.clear();
}

Player getCurrentPlayer(GameModel const&model)
{
    return model.tree.currentPlayer;
}

Player getCurrentPlayer(tree_logic const&tree)
{
    return tree.currentPlayer;
}

int getScore(GameModel &model, Player player)
{
    int score = 0;

    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (((model.tree.board[y][x] == PIECE_WHITE) &&
                 (player == PLAYER_WHITE)) ||
                ((model.tree.board[y][x] == PIECE_BLACK) &&
                 (player == PLAYER_BLACK)))
                score++;
        }

    return score;
}

int getScore(tree_logic const&tree, Player player)
{
    int score = 0;

    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (((tree.board[y][x] == PIECE_WHITE) &&
                 (player == PLAYER_WHITE)) ||
                ((tree.board[y][x] == PIECE_BLACK) &&
                 (player == PLAYER_BLACK)))
                score++;
        }

    return score;
}

double getTimer(GameModel &model, Player player)
{
    double turnTime = 0;

    if (!model.tree.gameOver && (player == model.tree.currentPlayer))
        turnTime = GetTime() - model.turnTimer;

    return model.playerTime[player] + turnTime;
}

Piece getBoardPiece(GameModel const&model, Square square)
{
    return model.tree.board[square.y][square.x];
}

Piece getBoardPiece(tree_logic const&tree, Square square)
{
    return tree.board[square.y][square.x];
}

void setBoardPiece(GameModel &model, Square square, Piece piece)
{
    model.tree.board[square.y][square.x] = piece;
}

void setBoardPiece(tree_logic &tree, Square square, Piece piece)
{
    tree.board[square.y][square.x] = piece;
}

bool isSquareValid(Square square)
{
    return (square.x >= 0) &&
           (square.x < BOARD_SIZE) &&
           (square.y >= 0) &&
           (square.y < BOARD_SIZE);
}

// Esta función revisa una dirección y nos dice cuántas fichas enemigas hay.
int checkDirection(GameModel const&model, Square start, int dx, int dy) {
    Piece playerPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;
    Piece opponentPiece = (getCurrentPlayer(model) == PLAYER_WHITE) ? PIECE_BLACK : PIECE_WHITE;

    int flipsInDirection = 0;
    Square currentPos = {start.x + dx, start.y + dy};

    // Avanzamos mientras estemos en el tablero y encontremos fichas del oponente.
    while (isSquareValid(currentPos) && getBoardPiece(model, currentPos) == opponentPiece) {
        flipsInDirection++;
        currentPos.x += dx;
        currentPos.y += dy;
    }

    // Un sándwich solo es válido si encontramos fichas para voltear (flipsInDirection > 0)
    // Y si la línea termina en una de nuestras fichas.
    if (flipsInDirection > 0 && isSquareValid(currentPos) && getBoardPiece(model, currentPos) == playerPiece) {
        return flipsInDirection;
    }

    // Si no se cumplen AMBAS condiciones, no es una jugada válida en esta dirección.
    return 0;
}


int checkDirection(tree_logic const&tree, Square start, int dx, int dy) {
    Piece playerPiece = (getCurrentPlayer(tree) == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;
    Piece opponentPiece = (getCurrentPlayer(tree) == PLAYER_WHITE) ? PIECE_BLACK : PIECE_WHITE;

    int flipsInDirection = 0;
    Square currentPos = {start.x + dx, start.y + dy};

    // Avanzamos mientras estemos en el tablero y encontremos fichas del oponente.
    while (isSquareValid(currentPos) && getBoardPiece(tree, currentPos) == opponentPiece) {
        flipsInDirection++;
        currentPos.x += dx;
        currentPos.y += dy;
    }

    // Un sándwich solo es válido si encontramos fichas para voltear (flipsInDirection > 0)
    // Y si la línea termina en una de nuestras fichas.
    if (flipsInDirection > 0 && isSquareValid(currentPos) && getBoardPiece(tree, currentPos) == playerPiece) {
        return flipsInDirection;
    }

    // Si no se cumplen AMBAS condiciones, no es una jugada válida en esta dirección.
    return 0;
}

void getValidMoves(GameModel const&model, Moves &validMoves)
{   
    Player player = model.tree.currentPlayer;

    for(int i=0; i<BOARD_SIZE; i++){
        for(int j=0; j<BOARD_SIZE; j++){
            Square currentSquare = {i, j};

            //Si la casilla no está vacía, seguimos.
            if(getBoardPiece(model, currentSquare) != PIECE_EMPTY){
                continue;
            }

            int totalEnemies = 0;

            //Se revisan las 8 direcciones.
            for(int dx = -1; dx <= 1; dx++){
                for(int dy = -1; dy <= 1; dy++){
                    //Nos saltamos la dirección (0,0) que es la que quiero analizar los "vecinos".
                    if(dx == 0 && dy == 0){
                        continue;
                    }

                    totalEnemies += checkDirection(model, currentSquare, dx, dy);   //revisamos si la dirección tiene fichas enemigas.
                }
            }

            if(totalEnemies > 0){
                validMoves.push_back(currentSquare);
            }

        }
    }
}

void getValidMoves(tree_logic const&tree, Moves &validMoves)
{   
    Player player = tree.currentPlayer;

    for(int i=0; i<BOARD_SIZE; i++){
        for(int j=0; j<BOARD_SIZE; j++){
            Square currentSquare = {i, j};

            //Si la casilla no está vacía, seguimos.
            if(getBoardPiece(tree, currentSquare) != PIECE_EMPTY){
                continue;
            }

            int totalEnemies = 0;

            //Se revisan las 8 direcciones.
            for(int dx = -1; dx <= 1; dx++){
                for(int dy = -1; dy <= 1; dy++){
                    //Nos saltamos la dirección (0,0) que es la que quiero analizar los "vecinos".
                    if(dx == 0 && dy == 0){
                        continue;
                    }

                    totalEnemies += checkDirection(tree, currentSquare, dx, dy);   //revisamos si la dirección tiene fichas enemigas.
                }
            }

            if(totalEnemies > 0){
                validMoves.push_back(currentSquare);
            }

        }
    }
}

bool playMove(GameModel &model, Square move)
{
    // Set game piece
    Piece piece =
        (getCurrentPlayer(model) == PLAYER_WHITE)
            ? PIECE_WHITE
            : PIECE_BLACK;

    setBoardPiece(model, move, piece);
    model.moveHistory.push_back(move);

    for(int dx = -1; dx <= 1; dx++){
        for(int dy = -1; dy <= 1; dy++){
            //Nos saltamos la dirección (0,0) que es la que quiero analizar los "vecinos".
            if(dx == 0 && dy == 0){
                continue;
            }

            int enemies = checkDirection(model, move, dx, dy);  //Si encontramos "enemigos" en esa dirección, 
                                                                //nos devuelve cuántas fichas hay que voltear.
            if(enemies > 0){
                Square currentPos = {move.x + dx, move.y + dy};

                for(int i = 0; i < enemies; i++){
                    setBoardPiece(model, currentPos, piece);
                    currentPos.x += dx;
                    currentPos.y += dy;
                }
            }
        }
    }
    // Update timer
    double currentTime = GetTime();
    model.playerTime[model.tree.currentPlayer] += currentTime - model.turnTimer;

    model.turnTimer = currentTime;

    //Update flags
    model.first_human_try = true;

    // Swap player
    model.tree.currentPlayer =
        (model.tree.currentPlayer == PLAYER_WHITE)
            ? PLAYER_BLACK
            : PLAYER_WHITE;

    // Game over?
    Moves validMoves;
    getValidMoves(model, validMoves);

    if (validMoves.size() == 0)
    {
        // Swap player
        model.tree.currentPlayer =
            (model.tree.currentPlayer == PLAYER_WHITE)
                ? PLAYER_BLACK
                : PLAYER_WHITE;

        validMoves.clear();
        getValidMoves(model, validMoves);

        if (validMoves.size() == 0)
            model.tree.gameOver = true;
    }

    //reseteo valid moves
    model.human_moves.clear();

    return true;
}

bool playMove(tree_logic &tree, Square move)
{
    // Set game piece
    Piece piece =
        (getCurrentPlayer(tree) == PLAYER_WHITE)
            ? PIECE_WHITE
            : PIECE_BLACK;

    setBoardPiece(tree, move, piece);

    for(int dx = -1; dx <= 1; dx++){
        for(int dy = -1; dy <= 1; dy++){
            //Nos saltamos la dirección (0,0) que es la que quiero analizar los "vecinos".
            if(dx == 0 && dy == 0){
                continue;
            }

            int enemies = checkDirection(tree, move, dx, dy);  //Si encontramos "enemigos" en esa dirección, 
                                                                //nos devuelve cuántas fichas hay que voltear.
            if(enemies > 0){
                Square currentPos = {move.x + dx, move.y + dy};

                for(int i = 0; i < enemies; i++){
                    setBoardPiece(tree, currentPos, piece);
                    currentPos.x += dx;
                    currentPos.y += dy;
                }
            }
        }
    }

    // Swap player
    tree.currentPlayer =
        (tree.currentPlayer == PLAYER_WHITE)
            ? PLAYER_BLACK
            : PLAYER_WHITE;

    // Game over?
    Moves validMoves;
    getValidMoves(tree, validMoves);

    if (validMoves.size() == 0)
    {
        // Swap player
        tree.currentPlayer =
            (tree.currentPlayer == PLAYER_WHITE)
                ? PLAYER_BLACK
                : PLAYER_WHITE;

        validMoves.clear();
        getValidMoves(tree, validMoves);

        if (validMoves.size() == 0)
            tree.gameOver = true;
    }

    return true;
}

tree_logic gameStateFromModel(GameModel const& model)
{
    tree_logic state;
    state.currentPlayer = model.tree.currentPlayer;
    state.gameOver = model.tree.gameOver;
    memcpy(state.board, model.tree.board, sizeof(state.board));
    return state;
}