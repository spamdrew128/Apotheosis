#include "endings.h"
#include "legals.h"
#include "zobrist.h"

GameEndStatus_t CurrentGameEndStatus(
    BoardInfo_t* boardInfo,
    GameState_t* gameState,
    ZobristStack_t* zobristStack,
    ZobristHash_t positionHash,
    int moveListMaxIndex,
    Color_t colorToMove
) 
{
    if(moveListMaxIndex == movelist_empty) {
        if(boardInfo->kings[colorToMove] & UnsafeSquares(boardInfo, colorToMove)) {
            return checkmate;
        } else {
            return draw;
        }
    }

    if(gameState->halfmoveClock >= 100) {
        return draw;
    }

    return ongoing;
}