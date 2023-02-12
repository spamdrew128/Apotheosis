#include <stdbool.h>

#include "endings.h"
#include "legals.h"
#include "zobrist.h"

static bool IsThreefoldRepetition(
    ZobristStack_t* zobristStack,
    ZobristHash_t positionHash,
    HalfmoveCount_t halfmoves
)
{
    int start = zobristStack->maxIndex - halfmoves;

    int hashOccurances = 1; // starts at one because positionHash hasn't been added
    for(int i = start; i <= zobristStack->maxIndex; i++) {
        if(zobristStack->entries[i] == positionHash) {
            hashOccurances++;
        }
    }

    return hashOccurances >= 3;
}

GameEndStatus_t CurrentGameEndStatus(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
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

    HalfmoveCount_t halfmoves = ReadHalfmoveClock(gameStack);
    if(halfmoves >= 100) {
        return draw;
    }

    if(IsThreefoldRepetition(zobristStack, positionHash, halfmoves)) {
        return draw;
    }

    return ongoing;
}