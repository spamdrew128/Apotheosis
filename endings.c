#include <stdbool.h>

#include "endings.h"
#include "legals.h"
#include "zobrist.h"

static bool IsThreefoldRepetition(
    ZobristStack_t* zobristStack,
    HalfmoveCount_t halfmoves
)
{
    ZobristHash_t currentPositionHash = zobristStack->entries[zobristStack->maxIndex];
    int searchStart = zobristStack->maxIndex - 2;
    int searchEnd = zobristStack->maxIndex - halfmoves;

    int hashOccurances = 1; // starts at one because currentPositionHash counts!
    for(int i = searchStart; i >= searchEnd; i -= 2) {
        if(zobristStack->entries[i] == currentPositionHash) {
            hashOccurances++;
        }
    }

    return hashOccurances >= 3;
}

static bool OnlyMinorPiecesOnBoard(BoardInfo_t* boardInfo) {
    return 
        (boardInfo->pawns[white] == empty_set) &&
        (boardInfo->pawns[black] == empty_set) &&
        (boardInfo->rooks[white] == empty_set) &&
        (boardInfo->rooks[black] == empty_set) &&
        (boardInfo->queens[white] == empty_set) &&
        (boardInfo->queens[black] == empty_set);
}

static bool AtMostOneMinorPiece(BoardInfo_t* boardInfo, Color_t color) {
    return 
        PopCount(boardInfo->bishops[color] | boardInfo->knights[color]) <= 1;
}

static bool IsInsufficientMaterialDraw(BoardInfo_t* boardInfo) {
    return
        OnlyMinorPiecesOnBoard(boardInfo) &&
        AtMostOneMinorPiece(boardInfo, white) &&
        AtMostOneMinorPiece(boardInfo, black);
}

GameEndStatus_t CurrentGameEndStatus(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
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
    if(IsThreefoldRepetition(zobristStack, halfmoves)) {
        return draw;
    }

    if(IsInsufficientMaterialDraw(boardInfo)) {
        return draw;
    }

    if(halfmoves >= 100) {
        return draw;
    }

    return ongoing;
}