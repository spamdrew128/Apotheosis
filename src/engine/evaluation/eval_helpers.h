#ifndef __EVAL_HELPERS_H__
#define __EVAL_HELPERS_H__

#include <stdbool.h>
#include <stdint.h>

#include "board_constants.h"
#include "bitboards.h"
#include "board_info.h"
#include "engine_types.h"
#include "eval_constants.h"

Bitboard_t WhiteFill(Bitboard_t b);

Bitboard_t BlackFill(Bitboard_t b);

Bitboard_t WhiteForwardFill(Bitboard_t b);

Bitboard_t BlackForwardFill(Bitboard_t b);

Bitboard_t FileFill(Bitboard_t b);

void SerializeBySquare(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    Centipawns_t* mgScore,
    Centipawns_t* egScore,
    Centipawns_t bonus[NUM_PHASES][NUM_SQUARES]
);

void SerializeByFile(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    Centipawns_t* mgScore,
    Centipawns_t* egScore,
    Centipawns_t bonus[NUM_PHASES][NUM_FILES]
);

void SerializeByRank(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    Centipawns_t* mgScore,
    Centipawns_t* egScore,
    Centipawns_t bonus[NUM_PHASES][NUM_RANKS]
);

#endif