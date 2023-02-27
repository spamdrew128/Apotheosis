#include <assert.h>

#include "move_ordering.h"
#include "evaluation.h"

static EvalScore_t MVVScore(BoardInfo_t* boardInfo, Move_t capture) {
    Square_t toSquare = ReadToSquare(capture);
    Square_t fromSquare = ReadFromSquare(capture);

    Piece_t victim = PieceOnSquare(boardInfo, toSquare);
    Piece_t attacker = PieceOnSquare(boardInfo, fromSquare);
    
    assert(victim != none_type || ReadSpecialFlag(capture) == en_passant_flag);

    return ValueOfPiece(victim) - ValueOfPiece(attacker);
}

static void InsertionSortCaptures(MoveList_t* moveList, BoardInfo_t* boardInfo) {
    int maxIndex = moveList->maxCapturesIndex;
    for(int i = 1; i <= maxIndex; i++) {
        Move_t capture = moveList->moves[i];
        EvalScore_t captureScore = MVVScore(boardInfo, capture);

        int j = i - 1;
        while (j >= 0 && (captureScore > MVVScore(boardInfo, moveList->moves[j]))) {
            moveList->moves[j+1] = moveList->moves[j]; // shift to the right
            j--;
        }
        moveList->moves[j+1] = capture;
    }
}

void SortMoveList(MoveList_t* moveList, BoardInfo_t* boardInfo) {
    InsertionSortCaptures(moveList, boardInfo);
}