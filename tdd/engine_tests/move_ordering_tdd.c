#include "move_ordering_tdd.h"
#include "game_state.h"
#include "zobrist.h"
#include "debug.h"
#include "FEN.h"
#include "evaluation.h"
#include "move.h"
#include "killers.h"

static MoveList_t moveList;
static BoardInfo_t boardInfo;
static GameStack_t gameStack;
static ZobristStack_t zobristStack;

enum {
    some_ply = 0,
    some_depth = 5,
};

bool IsCapture(Move_t move) {
    Piece_t victim = PieceOnSquare(&boardInfo, ReadToSquare(move));
    return victim != none_type || ReadSpecialFlag(move) == en_passant_flag;
}

static EvalScore_t MVVScore(Move_t capture) {
    Square_t toSquare = ReadToSquare(capture);
    Square_t fromSquare = ReadFromSquare(capture);

    Piece_t victim = PieceOnSquare(&boardInfo, toSquare);
    Piece_t attacker = PieceOnSquare(&boardInfo, fromSquare);

    return ValueOfPiece(victim) - ValueOfPiece(attacker);
}

static MoveScore_t AssignTestScore(Move_t move, Move_t ttMove, Killers_t* killers, History_t* history, Ply_t ply) {
    if(CompareMoves(move, ttMove)) {
        return tt_score;
    } else if(ReadSpecialFlag(move) == promotion_flag) {
        return promotion_score;
    } else if(IsCapture(move)) {
        return MVVScore(move) + capture_offset;
    } else if(CompareMoves(move, GetKiller(killers, ply, 0))) {
        return killer_base_score;
    } else if(CompareMoves(move, GetKiller(killers, ply, 1))) {
        return killer_base_score - 1;
    } else {
        return HistoryScore(history, &boardInfo, move);
    }
}

static bool MovesAreCorrectlyOrdered(MovePicker_t* picker, Move_t ttMove, Killers_t* killers, History_t* history, Ply_t ply) {
    Move_t prevMove = PickMove(picker);
    for(int i = 1; i <= moveList.maxIndex; i++) {
        Move_t currentMove = PickMove(picker);
        if(AssignTestScore(prevMove, ttMove, killers, history, ply) < 
            AssignTestScore(currentMove, ttMove, killers, history, ply)
        )
        {
            return false;
        }
        prevMove = currentMove;
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

    Killers_t killers;
    InitKillers(&killers);

    Move_t killer0 = NullMove();
    WriteFromSquare(&killer0, a2);
    WriteToSquare(&killer0, a4);

    Move_t killer1 = NullMove();
    WriteFromSquare(&killer1, a2);
    WriteToSquare(&killer1, a3);

    AddKiller(&killers, killer0, some_ply);
    AddKiller(&killers, killer1, some_ply);

    History_t history;
    InitHistory(&history);

    Move_t historyMove = NullMove();
    WriteFromSquare(&historyMove, h1);
    WriteToSquare(&historyMove, g1);
    UpdateHistory(&history, &boardInfo, historyMove, some_depth);

    InitAllMovePicker(&picker, &moveList, &boardInfo, ttMove, &killers, &history, some_ply);

    PrintResults(MovesAreCorrectlyOrdered(&picker, ttMove, &killers, &history, some_ply));
}

void MoveOrderingTDDRunner() {
    ShouldOrderCorrectly();
}
