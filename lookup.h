#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include "board_constants.h"
#include "bitboards.h"

typedef struct {
    Bitboard_t singleBitsets[63];
} Lookup_t;

void InitLookup();

Bitboard_t GetSingleBitset(int square);

#endif