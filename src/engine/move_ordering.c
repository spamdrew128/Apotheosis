#include <assert.h>
#include <limits.h>

#include "move_ordering.h"
#include "evaluation.h"

enum IteratorScoreValues {
    tt_move_score = INT32_MAX,
    quiet_move_score = INT32_MIN,
};

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

void MoveListIteratorInit(MoveListIterator_t* iterator, MoveList_t* moveList, BoardInfo_t* boardInfo, int maxIndex) {
    for(int i = 0; i <= maxIndex; i++) {
        Move_t move = moveList->moves[i];
        iterator->itemList[i].move = move;

        if(i <= moveList->maxCapturesIndex) {
            iterator->itemList[i].score = MVVScore(boardInfo, move);
        } else {
            iterator->itemList[i].score = quiet_move_score;
        }
    }
    iterator->headIndex = 0;
    iterator->maxIndex = maxIndex;
}

static void RemoveIteratorItem(MoveListIterator_t* iterator, int removedIndex) {
    MoveListIteratorItem_t headItem = iterator->itemList[iterator->headIndex];
    
    iterator->itemList[iterator->headIndex] = iterator->itemList[removedIndex];
    iterator->itemList[removedIndex] = headItem;

    iterator->headIndex++;
}

Move_t IteratorGetNextMove(MoveListIterator_t* iterator) {
    int bestMoveIndex = iterator->headIndex;
    IteratorScore_t bestScore = iterator->itemList[bestMoveIndex].score;

    for(int i = iterator->headIndex + 1; i <= iterator->maxIndex; i++) {
        IteratorScore_t score = iterator->itemList[i].score;
        if(score > bestScore) {
            bestScore = score;
            bestMoveIndex = i;
        }
    }

    Move_t nextMove = iterator->itemList[bestMoveIndex].move;
    RemoveIteratorItem(iterator, bestMoveIndex);

    return nextMove;
}

void SortMoveList(MoveList_t* moveList, BoardInfo_t* boardInfo) {
    InsertionSortCaptures(moveList, boardInfo);
}