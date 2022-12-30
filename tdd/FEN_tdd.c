#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "FEN_tdd.h"
#include "FEN.h"
#include "bitboards.h"

void FENTDDRunner() {
    BoardInfo_t info;

    InitBoardInfo(&info);
    InterpretFEN(START_FEN, &info);
    // PrintChessboard(&info);

    InitBoardInfo(&info);
    InterpretFEN("r1b1qrk1/pp2np1p/2pp1npQ/3Pp1P1/4P3/2N2N2/PPP2P2/2KR1B1R", &info);
    // PrintChessboard(&info);

    // PrintBitboard(info.allPieces[white] | info.allPieces[black]);
}