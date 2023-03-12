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

void InitAllMovePicker(
    MovePicker_t* movePicker,
    MoveList_t* moveList,
    BoardInfo_t* boardInfo,
    Move_t ttMove,
    Killers_t* killers,
    History_t* history,
    Ply_t ply
)
{
    movePicker->headIndex = 0;
    movePicker->moveList = moveList;
    movePicker->tailIndex = moveList->maxIndex;

    Move_t killer_0 = GetKiller(killers, ply, 0);
    Move_t killer_1 = GetKiller(killers, ply, 1);

    for(MoveIndex_t i = 0; i <= movePicker->tailIndex; i++) {
        Move_t move = moveList->moves[i].move;
        if(CompareMoves(move, ttMove)) {
            moveList->moves[i].score = tt_score;
        } else if(ReadSpecialFlag(move) == promotion_flag) {
            moveList->moves[i].score = promotion_score;
        } else if(i <= moveList->maxCapturesIndex) {
            moveList->moves[i].score = MVVScore(boardInfo, move) + capture_offset;
        } else if(CompareMoves(move, killer_0)) {
            moveList->moves[i].score = killer_base_score;
        } else if(CompareMoves(move, killer_1)) {
            moveList->moves[i].score = killer_base_score - 1;
        } else {
            moveList->moves[i].score = HistoryScore(history, boardInfo, move);
            assert(moveList->moves[i].score < killer_base_score);
        }
    }
}

void InitCaptureMovePicker(
    MovePicker_t* movePicker,
    MoveList_t* moveList,
    BoardInfo_t* boardInfo
)
{
    movePicker->headIndex = 0;
    movePicker->moveList = moveList;
    movePicker->tailIndex = moveList->maxCapturesIndex;

    for(MoveIndex_t i = 0; i <= movePicker->tailIndex; i++) {
        Move_t move = moveList->moves[i].move;
        if(ReadSpecialFlag(move) == promotion_flag) {
            moveList->moves[i].score = promotion_score;
        } else {
            moveList->moves[i].score = MVVScore(boardInfo, move);
        }
    }
}

static void SwapEntries(MoveList_t* moveList, MoveIndex_t i, MoveIndex_t j) {
    MoveEntry_t temp = moveList->moves[i];
    moveList->moves[i] = moveList->moves[j];
    moveList->moves[j] = temp;
}

Move_t PickMove(MovePicker_t* movePicker) {
    MoveList_t* moveList = movePicker->moveList;
    MoveIndex_t head = movePicker->headIndex;

    assert(head <= movePicker->tailIndex);

    MoveIndex_t bestMoveIndex = head;
    MoveScore_t bestScore = moveList->moves[head].score;

    for(MoveIndex_t i = head+1; i <= movePicker->tailIndex; i++) {
        MoveScore_t score = moveList->moves[i].score;
        if(score > bestScore) {
            bestScore = score;
            bestMoveIndex = i;
        }
    }

    Move_t bestMove = moveList->moves[bestMoveIndex].move;
    SwapEntries(moveList, head, bestMoveIndex);
    movePicker->headIndex++;

    return bestMove;
}