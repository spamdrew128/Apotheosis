#include "move_ordering_tdd.h"
#include "game_state.h"
#include "zobrist.h"
#include "debug.h"
#include "FEN.h"
#include "evaluation.h"

static MoveList_t moveList;
static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

static EvalScore_t MVVScore(Move_t capture) {
    Square_t toSquare = ReadToSquare(capture);
    Square_t fromSquare = ReadFromSquare(capture);

    Piece_t victim = PieceOnSquare(&boardInfo, toSquare);
    Piece_t attacker = PieceOnSquare(&boardInfo, fromSquare);

    return ValueOfPiece(victim) - ValueOfPiece(attacker);
}

static bool MoveIsCapture(Move_t capture) {
    Piece_t victim = PieceOnSquare(&boardInfo, ReadToSquare(capture));
    
    return (victim != none_type || ReadSpecialFlag(capture) == en_passant_flag);
}

static bool CapturesAreCorrectlyOrdered(MoveListIterator_t* iterator) {
    Move_t prevMove = IteratorGetNextMove(iterator);

    for(int i = 1; i <= moveList.maxCapturesIndex; i++) {
        Move_t currentMove = IteratorGetNextMove(iterator);

        if(!MoveIsCapture(prevMove) || !MoveIsCapture(currentMove)) {
            return false;
        }

        if(MVVScore(prevMove) < MVVScore(currentMove)) {
            return false;
        }

        prevMove = currentMove;
    }

    return true;
}

static void ShouldOrderCaptures() {
    FEN_t manyCapturesFen = "rnb1kb1r/p4ppp/2p5/4N3/1ppqP1n1/2P1BQ1P/PP3PP1/RN2K2R w KQkq - 2 10";
    InterpretFEN(manyCapturesFen, &boardInfo, &gameStack, &zobristStack);
    CompleteMovegen(&moveList, &boardInfo, &gameStack);
    
    MoveListIterator_t iterator;
    MoveListIteratorInit(&iterator, &moveList, &boardInfo, moveList.maxIndex);

    PrintResults(CapturesAreCorrectlyOrdered(&iterator));
}

void MoveOrderingTDDRunner() {
    ShouldOrderCaptures();
}
