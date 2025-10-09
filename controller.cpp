/**
 * @brief Implements the Reversi game controller
 * @author Marc S. Ressl
 *
 * @copyright Copyright (c) 2023-2024
 */

#include <algorithm>

#include "raylib.h"

#include "ai.h"
#include "view.h"
#include "controller.h"

bool updateView(GameModel &model)
{
    if (WindowShouldClose())
        return false;

    if (model.gameOver)
    {
        if (IsMouseButtonPressed(0))
        {
            if (isMousePointerOverPlayBlackButton())
            {
                model.humanPlayer = PLAYER_BLACK;

                startModel(model);
            }
            else if (isMousePointerOverPlayWhiteButton())
            {
                model.humanPlayer = PLAYER_WHITE;

                startModel(model);
            }
        }
    }
    else if (model.currentPlayer == model.humanPlayer)
    {
        if(model.first_human_try){      //clausula para que no llame a la funcion getValidMoves innecesariamente
            getValidMoves(model, model.human_moves);
            model.first_human_try = false;
        }
        if (IsMouseButtonPressed(0))
        {
            // Human player
            Square square = getSquareOnMousePointer();
            if (isSquareValid(square))
            {  
                // Play move if valid
                for (auto move : model.human_moves)
                {
                    if ((square.x == move.x) &&
                        (square.y == move.y)){
                        playMove(model, square);
                    }
                }
            }
        }
    }
    else
    {
        // AI player
        Square square = getBestMove(model);

        playMove(model, square);
    }

    if ((IsKeyDown(KEY_LEFT_ALT) ||
         IsKeyDown(KEY_RIGHT_ALT)) &&
        IsKeyPressed(KEY_ENTER))
        ToggleFullscreen();

    drawView(model, model.human_moves);
    return true;
}
