/**
 * @brief Implements the Reversi game model
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <cstring>
#include <stdio.h>

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

/*void getValidMoves(GameModel &model, Moves &validMoves)
{   
    bool isPieceValid;
    Player player = getCurrentPlayer(model);
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            Square move = {x, y};
            if (getBoardPiece(model, move) == PIECE_EMPTY)
            { 
                for(auto i = x-1 ; i <= x + 1 && i < BOARD_SIZE && !isPieceValid ; i++)
                {
                    if(i < 0) continue;

                    for(auto j = y - 1; j <= y + 1 && j < BOARD_SIZE && !isPieceValid; j++){

                        if(j < 0 || (i == x && j == y)) continue;

                        Square piecePos = {i, j};

                        isPieceValid = false;

                        if (getBoardPiece(model, piecePos) != (player+1) && getBoardPiece(model, piecePos) != PIECE_EMPTY){ //player+1 = PICE_COLOR,
                            //case y fixed derech
                            printf("entro 1\n");
                            for(auto k = i + 1; k < BOARD_SIZE && !isPieceValid; k++){
                                Square pos={k, y};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                            printf("entro 2\n");
                            //case y fixed izq
                            for(auto k = x-1; k>=0 && !isPieceValid; k--){
                                Square pos={k, y};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                            //case x fixed derech
                            for(auto k = y+1; k<BOARD_SIZE && !isPieceValid; k++){
                                Square pos={x, k};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                            //case x fixed izq
                            for(auto k = y-1; k>=0 && !isPieceValid; k--){
                                Square pos={x, k};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                            //case x+1, y+1
                            for(auto k = x+1, q= y+1; q < BOARD_SIZE && k < BOARD_SIZE && !isPieceValid; k++, q++){
                                Square pos={k, q};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                            //case x-1, y-1
                            for(auto k = x-1, q= y-1; q >= 0 && k >= 0 && !isPieceValid; k--, q--){
                                Square pos={k, q};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                            //case x-1, y+1
                            for(auto k = x - 1, q = y + 1; k>=0 && q < BOARD_SIZE && !isPieceValid; k--, q++){
                                Square pos={k,q};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }
                            //case x+1, y-1
                            for(auto k = x + 1, q = y - 1; q >= 0 && k < BOARD_SIZE && !isPieceValid; q--, k++){
                                Square pos={k,q};
                                Piece color=getBoardPiece(model, pos);
                                if(color != PIECE_EMPTY){
                                    if(color == player+1){
                                        isPieceValid = true;
                                    }
                                }
                                else{
                                    break;
                                }
                            }

                        }
                    }
                }
                
            }
            if(isPieceValid)
            {
                validMoves.push_back(move);
            }    
        }
    }
}
    */

bool playMove(GameModel &model, Square move)
{
    // Set game piece
    Piece piece =
        (getCurrentPlayer(model) == PLAYER_WHITE)
            ? PIECE_WHITE
            : PIECE_BLACK;

    setBoardPiece(model, move, piece);

    // To-do: your code goes here...

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
