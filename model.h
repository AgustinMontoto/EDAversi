/**
 * @brief Implements the Reversi game model
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#ifndef MODEL_H
#define MODEL_H

#include <cstdint>
#include <vector>

#define BOARD_SIZE 8


enum Player
{
    PLAYER_BLACK,
    PLAYER_WHITE,
};

enum Piece
{
    PIECE_EMPTY,
    PIECE_BLACK,
    PIECE_WHITE,
};

struct Square
{
    int x;
    int y;
};

#define GAME_INVALID_SQUARE \
    {                       \
        -1, -1              \
    }

typedef std::vector<Square> Moves;

struct tree_logic
{
    Player currentPlayer;
    bool gameOver;
    Piece board[BOARD_SIZE][BOARD_SIZE];
};


struct GameModel
{   
    bool first_human_try;
    tree_logic tree;
    double playerTime[2];
    double turnTimer;           //en segundos
    Player humanPlayer;
    Moves human_moves;
    std::vector<Square> moveHistory;
};



Square isValid (GameModel &model, Square piece, const int directions[2]);

/**
 * @brief Initializes a game model.
 *
 * @param model The game model.
 */
void initModel(GameModel &model);

/**
 * @brief Starts a game.
 *
 * @param model The game model.
 */
void startModel(GameModel &model);

/**
 * @brief Returns the model's current player.
 *
 * @param model The game model.
 * @return PLAYER_WHITE or PLAYER_BLACK.
 */
Player getCurrentPlayer(GameModel const&model);

/**
 * @brief Returns the current player from tree_logic.
 *
 * @param tree The tree logic state.
 * @return PLAYER_WHITE or PLAYER_BLACK.
 */
Player getCurrentPlayer(tree_logic const&tree);

/**
 * @brief Returns the model's current score.
 *
 * @param model The game model.
 * @param player The player (PLAYER_WHITE or PLAYER_BLACK).
 * @return The score.
 */
int getScore(GameModel &model, Player player);

/**
 * @brief Returns the score from tree_logic.
 *
 * @param tree The tree logic state.
 * @param player The player (PLAYER_WHITE or PLAYER_BLACK).
 * @return The score.
 */
int getScore(tree_logic const&tree, Player player);

/**
 * @brief Returns the game timer for a player.
 *
 * @param model The game model.
 * @param player The player (PLAYER_WHITE or PLAYER_BLACK).
 * @return The time in seconds.
 */
double getTimer(GameModel &model, Player player);

/**
 * @brief Return a model's piece.
 *
 * @param model The game model.
 * @param square The square.
 * @return The piece at the square.
 */
Piece getBoardPiece(GameModel const&model, Square square);

/**
 * @brief Return a piece from tree_logic.
 *
 * @param tree The tree logic state.
 * @param square The square.
 * @return The piece at the square.
 */
Piece getBoardPiece(tree_logic const&tree, Square square);

/**
 * @brief Sets a model's piece.
 *
 * @param model The game model.
 * @param square The square.
 * @param piece The piece to be set
 */
void setBoardPiece(GameModel &model, Square square, Piece piece);

/**
 * @brief Sets a piece in tree_logic.
 *
 * @param tree The tree logic state.
 * @param square The square.
 * @param piece The piece to be set
 */
void setBoardPiece(tree_logic &tree, Square square, Piece piece);

/**
 * @brief Checks whether a square is within the board.
 *
 * @param square The square.
 * @return True or false.
 */
bool isSquareValid(Square square);

/**
 * @brief Returns a list of valid moves for the current player.
 *
 * @param model The game model.
 * @param validMoves A list that receives the valid moves.
 */
void getValidMoves(GameModel const&model, Moves &validMoves);

/**
 * @brief Returns a list of valid moves from tree_logic.
 *
 * @param tree The tree logic state.
 * @param validMoves A list that receives the valid moves.
 */
void getValidMoves(tree_logic const&tree, Moves &validMoves);

/**
 * @brief Plays a move.
 *
 * @param model The game model.
 * @param square The move.
 * @return Move accepted.
 */
bool playMove(GameModel &model, Square move);

/**
 * @brief Plays a move on tree_logic.
 *
 * @param tree The tree logic state.
 * @param square The move.
 * @return Move accepted.
 */
bool playMove(tree_logic &tree, Square move);

/**
 * @brief Checks the amount of enemy pieces around an empty square.
 *
 * @param model The game model.
 * @param start The square where we want to make a move.
 * @param dx Direction x.
 * @param dy Direction y.
 * @return Amount of surrounding enemy pieces.
 */
int checkDirection(GameModel const&model, Square start, int dx, int dy);

/**
 * @brief Checks the amount of enemy pieces from tree_logic.
 *
 * @param tree The tree logic state.
 * @param start The square where we want to make a move.
 * @param dx Direction x.
 * @param dy Direction y.
 * @return Amount of surrounding enemy pieces.
 */
int checkDirection(tree_logic const&tree, Square start, int dx, int dy);

/**
 * @brief Creates a tree_logic from GameModel.
 *
 * @param model The game model.
 * @return A tree_logic with the essential state.
 */
tree_logic gameStateFromModel(GameModel const& model);


#endif