#include <stdio.h>
#include <stdbool.h>

#include "debug.h"
#include "FEN_tdd.h"
#include "FEN.h"
#include "bitboards.h"

void FENTDDRunner() {
    BoardInfo_t info;

    InterpretFEN(START_FEN, &info);
}