#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "debug.h"
#include "movegen_tdd.h"
#include "pieces.h"
#include "board_constants.h"
#include "lookup.h"

// HELPERS
static int CountPieceMoves(Piece_t piece, MoveList_t moveList) {
    int count = 0;
    for(int i = 0; i < moveList.numMoves; i++) {
        if(moveList.moves[i].piece == piece) {
            count++;
        }
    }

    return count;
}

// q5bk/1P6/2P1Q3/3K2Rr/8/3N1B2/3n4/3r4
static void InitPinPositionInfo(BoardInfo_t* info) {
    InitBoardInfo(info);
    info->kings[white] = CreateBitboard(1, d5);
    info->pawns[white] = CreateBitboard(2, c6,b7);
    info->rooks[white] = CreateBitboard(1, g5);
    info->bishops[white] = CreateBitboard(1, f3);\
    info->knights[white] = CreateBitboard(1, d3);
    info->queens[white] = CreateBitboard(1, e6);

    info->kings[black] = CreateBitboard(1, h8);
    info->knights[black] = CreateBitboard(1, d2);
    info->bishops[black] = CreateBitboard(1, g8);
    info->rooks[black] = CreateBitboard(2, d1,h5);
    info->queens[black] = CreateBitboard(1, a8);

    UpdateAllPieces(info);
    UpdateEmpty(info);
}

// TESTS
static void ShouldCorrectlyEvaluatePosWithPins() {
    BoardInfo_t info;
    InitPinPositionInfo(&info);
    
    int expectedNumKingMoves = 4;
    int expectedNumPawnMoves = 3;
    int expectedNumRookMoves = 4;
    int expectedNumBishopMoves = 7;
    int expectedNumKnightsMoves = 8;
    int expectedNumQueenMoves = 2;

    MoveList_t moveList;

    bool success = 
        (CountPieceMoves(king, moveList) == expectedNumKingMoves) &&
        (CountPieceMoves(pawn, moveList) == expectedNumPawnMoves) &&
        (CountPieceMoves(rook, moveList) == expectedNumRookMoves) &&
        (CountPieceMoves(bishop, moveList) == expectedNumBishopMoves) &&
        (CountPieceMoves(knight, moveList) == expectedNumKnightsMoves) &&
        (CountPieceMoves(queen, moveList) == expectedNumQueenMoves);

    PrintResults(success);
}

void MovegenTDDRunner() {
    // ShouldCorrectlyEvaluatePosWithPins();
}