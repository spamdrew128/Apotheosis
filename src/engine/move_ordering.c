#include <assert.h>

#include "move_ordering.h"
#include "evaluation.h"

enum MoveScores {
    tt_score = ENTRY_MAX_SCORE,
    promotion_score = tt_score - 1,
    quiet_score = ENTRY_MIN_SCORE,
};

static EvalScore_t MVVScore(BoardInfo_t* boardInfo, Move_t capture) {
    Square_t toSquare = ReadToSquare(capture);
    Square_t fromSquare = ReadFromSquare(capture);

    Piece_t victim = PieceOnSquare(boardInfo, toSquare);
    Piece_t attacker = PieceOnSquare(boardInfo, fromSquare);
    
    assert(victim != none_type || ReadSpecialFlag(capture) == en_passant_flag);

    return ValueOfPiece(victim) - ValueOfPiece(attacker);
}

void InitMovePicker(
    MovePicker_t* movePicker,
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Move_t ttMove,
    MoveIndex_t finalIndex
)
{
    movePicker->headIndex = 0;
    movePicker->moveList = moveList;
    movePicker->tailIndex = finalIndex;

    for(MoveIndex_t i = 0; i <= finalIndex; i++) {
        Move_t move = moveList->moves[i].move;
        if(CompareMoves(move, ttMove)) {
            moveList->moves[i].score = tt_score;
        } else if(ReadSpecialFlag(move) == promotion_flag) {
            moveList->moves[i].score = promotion_score;
        } else if(i <= moveList->maxCapturesIndex) {
            moveList->moves[i].score = MVVScore(boardInfo, move);
        } else {
            moveList->moves[i].score = quiet_score;
        }
    }
}

void SortTTMove(MoveList_t* moveList, Move_t ttMove, int maxIndex) {
    for(int i = 0; i <= maxIndex; i++) {
        if(CompareMoves(ttMove, moveList->moves[i])) {
            for(int j = i; j > 0; j--) {
                moveList->moves[j] = moveList->moves[j-1];
            }
            moveList->moves[0] = ttMove;
            return;
        }
    }
}