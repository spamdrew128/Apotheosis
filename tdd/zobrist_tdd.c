#include "zobrist_tdd.h"
#include "debug.h"
#include "FEN.h"
#include "board_info.h"
#include "game_state.h"
#include "UCI.h"

static FEN_t someFen = "1k4r1/p2n1p2/Pp1p4/3P4/1RPr3p/6P1/5P1P/R4K2 b - - 1 26";
static BoardInfo_t info;
static GameStack_t stack;

static void ShouldGenerateNonZeroHash() {
    Color_t colorToMove = InterpretFEN(someFen, &info, &stack);
    GameState_t state = ReadCurrentGameState(&stack);
    ZobristHash_t hash = HashPosition(&info, &state, colorToMove);
    PrintResults(hash);
}

static void ShouldGenerateSameHashesForSamePositions() {
    FEN_t fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";  

    const char* transpositionTestTable[20][2][4] = {
        {{"e2c4", "h8f8", "d2h6", "b4b3"}, {"e2c4", "b4b3", "d2h6", "h8f8"}},
        {{"c3a4", "f6g8", "e1d1", "a8c8"}, {"c3a4", "a8c8", "e1d1", "f6g8"}},
        {{"h1g1", "f6g4", "d2h6", "b4b3"}, {"h1g1", "b4b3", "d2h6", "f6g4"}},
        {{"a1c1", "c7c5", "c3a4", "a6e2"}, {"c3a4", "c7c5", "a1c1", "a6e2"}},
        {{"e2c4", "h8h5", "f3f5", "e7d8"}, {"f3f5", "h8h5", "e2c4", "e7d8"}},
        {{"d5d6", "e8h8", "f3f6", "a6c4"}, {"f3f6", "a6c4", "d5d6", "e8h8"}},
        {{"f3e3", "e8h8", "a2a4", "a8c8"}, {"a2a4", "a8c8", "f3e3", "e8h8"}},
        {{"e1d1", "f6d5", "b2b3", "a8c8"}, {"e1d1", "a8c8", "b2b3", "f6d5"}},
        {{"e1d1", "e8f8", "e5c6", "h8h5"}, {"e1d1", "h8h5", "e5c6", "e8f8"}},
        {{"e2d3", "c7c6", "g2g4", "h8h6"}, {"e2d3", "h8h6", "g2g4", "c7c6"}},
        {{"f3h5", "f6h7", "c3b1", "g7f6"}, {"c3b1", "f6h7", "f3h5", "g7f6"}},
        {{"e2d3", "g6g5", "d2f4", "b6d5"}, {"d2f4", "g6g5", "e2d3", "b6d5"}},
        {{"a2a3", "h8h5", "c3b1", "a8d8"}, {"a2a3", "a8d8", "c3b1", "h8h5"}},
        {{"a2a4", "e8h8", "e1h1", "e7d8"}, {"e1h1", "e8h8", "a2a4", "e7d8"}},
        {{"b2b3", "e8f8", "g2g3", "a6b7"}, {"b2b3", "a6b7", "g2g3", "e8f8"}},
        {{"e5g4", "e8d8", "d2e3", "a6d3"}, {"d2e3", "a6d3", "e5g4", "e8d8"}},
        {{"g2h3", "e7d8", "e5g4", "b6c8"}, {"e5g4", "b6c8", "g2h3", "e7d8"}},
        {{"e5d3", "a6b7", "g2g3", "h8h6"}, {"e5d3", "h8h6", "g2g3", "a6b7"}},
        {{"e5g4", "h8h5", "f3f5", "e6f5"}, {"f3f5", "e6f5", "e5g4", "h8h5"}},
        {{"g2g3", "a8c8", "e5d3", "e7f8"}, {"e5d3", "a8c8", "g2g3", "e7f8"}}
    };
    


    for(int i = 0; i < 20; i++) {
        const char* moveSet1[4] = transpositionTestTable[i][0];
        const char* moveSet2[4] = transpositionTestTable[i][1];
        ZobristHash_t moveSet1Hash;
        ZobristHash_t moveSet2Hash;

        Color_t colorToMove = InterpretFEN(someFen, &info, &stack);
        GameState_t state = ReadCurrentGameState(&stack);
        for(int i = 0; i < 4; i++) {

        }
    }

}

void ZobristTDDRunner() {
    ShouldGenerateNonZeroHash();
}