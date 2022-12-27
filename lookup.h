#ifndef __LOOKUP_H__
#define __LOOKUP_H__

#include "board_constants.h"
#include "bitboards.h"

typedef struct {
    Bitboard_t singleBitset[63];
} Lookup_t;

void CreateLookup(Lookup_t* lookup);

#endif