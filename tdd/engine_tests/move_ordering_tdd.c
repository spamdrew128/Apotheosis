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

static EvalScore_t MVVScore(Move_t capture) {
    Square_t toSquare = ReadToSquare(capture);
    Square_t fromSquare = ReadFromSquare(capture);

    Piece_t victim = PieceOnSquare(&boardInfo, toSquare);
    Piece_t attacker = PieceOnSquare(&boardInfo, fromSquare);
    
    assert(victim != none_type);

    return ValueOfPiece(victim) - ValueOfPiece(attacker);
}

static bool CapturesAreCorrectlyOrdered() {
    for(int i = 1; i <= moveList.maxCapturesIndex; i++) {
        Move_t prevMove = moveList.moves[i-1];
        Move_t currentMove = moveList.moves[i];
        if(MVVScore(prevMove) < MVVScore(currentMove)) {
            return false;
        }
    }

    return true;
}

static void ShouldOrderCaptures() {
    FEN_t manyCapturesFen = "rnb1kb1r/p4ppp/2p5/4N3/1ppqP1n1/2P1BQ1P/PP3PP1/RN2K2R w KQkq - 2 10";
    InterpretFEN(manyCapturesFen, &boardInfo, &gameStack, &zobristStack);
    CompleteMovegen(&moveList, &boardInfo, &gameStack);
    SortCaptures(&moveList, &boardInfo);

    PrintResults(CapturesAreCorrectlyOrdered());
}

static void ShouldOrderTTFirst() {
    FEN_t someFen = "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3";
    Move_t ttMove;
    InitMove(&ttMove);
    WriteFromSquare(&ttMove, f1);
    WriteToSquare(&ttMove, d3);
    
    InterpretFEN(someFen, &boardInfo, &gameStack, &zobristStack);
    CompleteMovegen(&moveList, &boardInfo, &gameStack);
    SortTTMove(&moveList, ttMove, moveList.maxIndex);

    PrintResults(CompareMoves(moveList.moves[0], ttMove));
}

void MoveOrderingTDDRunner() {
    ShouldOrderCaptures();
    ShouldOrderTTFirst();
}
