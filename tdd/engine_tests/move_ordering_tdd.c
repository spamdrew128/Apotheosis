#include "move_ordering_tdd.h"
#include "game_state.h"
#include "zobrist.h"
#include "debug.h"
#include "FEN.h"
#include "evaluation.h"
#include "move.h"

static MoveList_t moveList;
static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

bool IsCapture(Move_t move) {
    Square_t toSquare = ReadToSquare(move);
    Square_t fromSquare = ReadFromSquare(move);

    Piece_t victim = PieceOnSquare(&boardInfo, toSquare);
    Piece_t attacker = PieceOnSquare(&boardInfo, fromSquare);
    return victim != none_type || ReadSpecialFlag(move) == en_passant_flag;
}

static EvalScore_t MVVScore(Move_t capture) {
    Square_t toSquare = ReadToSquare(capture);
    Square_t fromSquare = ReadFromSquare(capture);

    Piece_t victim = PieceOnSquare(&boardInfo, toSquare);
    Piece_t attacker = PieceOnSquare(&boardInfo, fromSquare);

    return ValueOfPiece(victim) - ValueOfPiece(attacker);
}

static MoveScore_t AssignTestScore(Move_t move, Move_t ttMove) {
    if(CompareMoves(move, ttMove)) {
        return tt_score;
    } else if(ReadSpecialFlag(move) == promotion_flag) {
        return 4;
    } else if(IsCapture(move)) {
        return MVVScore(move);
    } else {
        return quiet_score;
    }
}

static bool MovesAreCorrectlyOrdered(MovePicker_t* picker, Move_t ttMove) {
    for(int i = 1; i <= moveList.maxIndex; i++) {
        Move_t prevMove = PickMove(picker);
        Move_t currentMove = PickMove(picker);
        if(AssignTestScore(prevMove, ttMove) < AssignTestScore(currentMove, ttMove)) {
            return false;
        }
    }

    return true;
}

static void ShouldOrderCorrectly() {
    FEN_t manyCapturesFen = "rn2kb1r/p1P2ppp/2p5/4N3/1ppqP1n1/2P1BQ1P/PP3PP1/RN2K2R w KQkq - 2 10";
    InterpretFEN(manyCapturesFen, &boardInfo, &gameStack, &zobristStack);
    CompleteMovegen(&moveList, &boardInfo, &gameStack);

    MovePicker_t picker;
    Move_t ttMove = NullMove();
    WriteFromSquare(&ttMove, e3);
    WriteToSquare(&ttMove, f4);

    InitMovePicker(&picker, &moveList, &boardInfo, ttMove, moveList.maxIndex);

    PrintResults(MovesAreCorrectlyOrdered(&picker, ttMove));
}

void MoveOrderingTDDRunner() {
    ShouldOrderCaptures();
    ShouldOrderTTFirst();
}
