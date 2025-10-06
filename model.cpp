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
    model.gameOver = true;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;

    memset(model.board, PIECE_EMPTY, sizeof(model.board));
}

void startModel(GameModel &model)
{
    model.gameOver = false;

    model.currentPlayer = PLAYER_BLACK;

    model.playerTime[0] = 0;
    model.playerTime[1] = 0;
    model.turnTimer = GetTime();

    memset(model.board, PIECE_EMPTY, sizeof(model.board));
    model.board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2 - 1] = PIECE_WHITE;
    model.board[BOARD_SIZE / 2 - 1][BOARD_SIZE / 2] = PIECE_BLACK;
    model.board[BOARD_SIZE / 2][BOARD_SIZE / 2] = PIECE_WHITE;
    model.board[BOARD_SIZE / 2][BOARD_SIZE / 2 - 1] = PIECE_BLACK;
}

Player getCurrentPlayer(GameModel &model)
{
    return model.currentPlayer;
}

int getScore(GameModel &model, Player player)
{
    int score = 0;

    for (int y = 0; y < BOARD_SIZE; y++)
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (((model.board[y][x] == PIECE_WHITE) &&
                 (player == PLAYER_WHITE)) ||
                ((model.board[y][x] == PIECE_BLACK) &&
                 (player == PLAYER_BLACK)))
                score++;
        }

    return score;
}

double getTimer(GameModel &model, Player player)
{
    double turnTime = 0;

    if (!model.gameOver && (player == model.currentPlayer))
        turnTime = GetTime() - model.turnTimer;

    return model.playerTime[player] + turnTime;
}

Piece getBoardPiece(GameModel &model, Square square)
{
    return model.board[square.y][square.x];
}

void setBoardPiece(GameModel &model, Square square, Piece piece)
{
    model.board[square.y][square.x] = piece;
}

bool isSquareValid(Square square)
{
    return (square.x >= 0) &&
           (square.x < BOARD_SIZE) &&
           (square.y >= 0) &&
           (square.y < BOARD_SIZE);
}

// Esta función revisa una dirección y nos dice cuántas fichas enemigas hay.
int checkDirection(GameModel &model, Square start, int dx, int dy) {
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

void getValidMoves(GameModel &model, Moves &validMoves)
{   
    Player player = getCurrentPlayer(model);
    Piece playerColor = (player == PLAYER_WHITE) ? PIECE_WHITE : PIECE_BLACK;

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

bool playMove(GameModel &model, Square move)
{
    // Set game piece
    Piece piece =
        (getCurrentPlayer(model) == PLAYER_WHITE)
            ? PIECE_WHITE
            : PIECE_BLACK;

    setBoardPiece(model, move, piece);

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
    model.playerTime[model.currentPlayer] += currentTime - model.turnTimer;
    model.turnTimer = currentTime;

    // Swap player
    model.currentPlayer =
        (model.currentPlayer == PLAYER_WHITE)
            ? PLAYER_BLACK
            : PLAYER_WHITE;

    // Game over?
    Moves validMoves;
    getValidMoves(model, validMoves);

    if (validMoves.size() == 0)
    {
        // Swap player
        model.currentPlayer =
            (model.currentPlayer == PLAYER_WHITE)
                ? PLAYER_BLACK
                : PLAYER_WHITE;

        Moves validMoves;
        getValidMoves(model, validMoves);

        if (validMoves.size() == 0)
            model.gameOver = true;
    }

    return true;
}
