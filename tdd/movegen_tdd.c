#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "debug.h"
#include "movegen_tdd.h"
#include "pieces.h"
#include "board_constants.h"
#include "lookup.h"
#include "move.h"

// HELPERS
static int CountPieceMoves(Piece_t piece, MoveList_t moveList, BoardInfo_t* info) {
    int count = 0;
    for(int i = 0; i < moveList.maxIndex; i++) {
        Square_t fromSquare = ReadFromSquare(moveList.moves[i])
        if(PieceOnSquare(info, fromSquare) == piece) {
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
    TranslateBitboardsToMailbox(info);
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
    CompleteMovegen(&moveList, &info, white);

    bool success = 
        (CountPieceMoves(king, moveList, &info) == expectedNumKingMoves) &&
        (CountPieceMoves(pawn, moveList, &info) == expectedNumPawnMoves) &&
        (CountPieceMoves(rook, moveList, &info) == expectedNumRookMoves) &&
        (CountPieceMoves(bishop, moveList, &info) == expectedNumBishopMoves) &&
        (CountPieceMoves(knight, moveList, &info) == expectedNumKnightsMoves) &&
        (CountPieceMoves(queen, moveList, &info) == expectedNumQueenMoves);

    PrintResults(success);
}

void MovegenTDDRunner() {
    // ShouldCorrectlyEvaluatePosWithPins();
}