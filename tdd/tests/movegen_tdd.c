#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include "debug.h"
#include "movegen_tdd.h"
#include "pieces.h"
#include "board_constants.h"
#include "lookup.h"
#include "move.h"
#include "game_state.h"

// HELPERS
static GameStack_t stack;

static void TestSetup() {
    InitGameStack(&stack);
}

static void CompleteMovegenTestWrapper(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack, Color_t color) {
    boardInfo->colorToMove = color;
    CompleteMovegen(moveList, boardInfo, stack);
}

static void CapturesMovegenTestWrapper(MoveList_t* moveList, BoardInfo_t* boardInfo, GameStack_t* stack, Color_t color) {
    boardInfo->colorToMove = color;
    CompleteMovegen(moveList, boardInfo, stack);
    moveList->maxIndex = moveList->maxCapturesIndex;
}

static int CountPieceMoves(Piece_t piece, MoveList_t moveList, BoardInfo_t* info) {
    int count = 0;
    for(int i = 0; i <= moveList.maxIndex; i++) {
        Square_t fromSquare = ReadFromSquare(moveList.moves[i].move);
        if(PieceOnSquare(info, fromSquare) == piece) {
            count++;
        }
    }

    return count;
}

// q5bk/1P6/2P1Q3/3K2Rr/8/3N1B2/3n4/3r4
static void InitPinPositionInfo(BoardInfo_t* info) {
    InitTestInfo(info, {
        info->kings[white] = CreateBitboard(1, d5);
        info->pawns[white] = CreateBitboard(2, c6,b7);
        info->rooks[white] = CreateBitboard(1, g5);
        info->bishops[white] = CreateBitboard(1, f3);
        info->knights[white] = CreateBitboard(1, d3);
        info->queens[white] = CreateBitboard(1, e6);

        info->kings[black] = CreateBitboard(1, h8);
        info->knights[black] = CreateBitboard(1, d2);
        info->bishops[black] = CreateBitboard(1, g8);
        info->rooks[black] = CreateBitboard(2, d1,h5);
        info->queens[black] = CreateBitboard(1, a8);
    });

    GetEmptyNextGameState(&stack);
}

// TESTS
static void ShouldCorrectlyEvaluateCapturesInPosWithPins() {
    TestSetup();
    BoardInfo_t info;
    InitPinPositionInfo(&info);

    int expectedNumKingCaptures = 0;
    int expectedNumPawnCaptures = 4;
    int expectedNumRookCaptures = 1;
    int expectedNumBishopCaptures = 2;
    int expectedNumKnightsCaptures = 0;
    int expectedNumQueenCaptures = 1;

    MoveList_t moveList;
    CapturesMovegenTestWrapper(&moveList, &info, &stack, white);

    bool success = 
        (CountPieceMoves(king, moveList, &info) == expectedNumKingCaptures) &&
        (CountPieceMoves(pawn, moveList, &info) == expectedNumPawnCaptures) &&
        (CountPieceMoves(rook, moveList, &info) == expectedNumRookCaptures) &&
        (CountPieceMoves(bishop, moveList, &info) == expectedNumBishopCaptures) &&
        (CountPieceMoves(knight, moveList, &info) == expectedNumKnightsCaptures) &&
        (CountPieceMoves(queen, moveList, &info) == expectedNumQueenCaptures) &&
        moveList.maxIndex == 7;

    PrintResults(success);
}

static void ShouldCorrectlyEvaluateInPosWithPins() {
    TestSetup();
    BoardInfo_t info;
    InitPinPositionInfo(&info);
    
    int expectedNumKingMoves = 4;
    int expectedNumPawnMoves = 9;
    int expectedNumRookMoves = 3;
    int expectedNumBishopMoves = 7;
    int expectedNumKnightsMoves = 8;
    int expectedNumQueenMoves = 2;

    MoveList_t moveList;
    CompleteMovegenTestWrapper(&moveList, &info, &stack, white);

    int expectedMaxIndex = (
        expectedNumKingMoves +
        expectedNumPawnMoves +
        expectedNumRookMoves +
        expectedNumBishopMoves +
        expectedNumKnightsMoves +
        expectedNumQueenMoves -1);

    bool success = 
        (CountPieceMoves(king, moveList, &info) == expectedNumKingMoves) &&
        (CountPieceMoves(pawn, moveList, &info) == expectedNumPawnMoves) &&
        (CountPieceMoves(rook, moveList, &info) == expectedNumRookMoves) &&
        (CountPieceMoves(bishop, moveList, &info) == expectedNumBishopMoves) &&
        (CountPieceMoves(knight, moveList, &info) == expectedNumKnightsMoves) &&
        (CountPieceMoves(queen, moveList, &info) == expectedNumQueenMoves) &&
        moveList.maxIndex == expectedMaxIndex;

    PrintResults(success);
}

void MovegenTDDRunner() {
    ShouldCorrectlyEvaluateCapturesInPosWithPins();
    ShouldCorrectlyEvaluateInPosWithPins();
}