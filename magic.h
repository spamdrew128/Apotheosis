#ifndef __MAGIC_H__
#define __MAGIC_H__

#include <stdint.h>

#include "board_constants.h"

typedef Bitboard_t MagicBB_t;

typedef struct {
    Bitboard_t mask;
    MagicBB_t magic;
    uint8_t shift;
    Bitboard_t* hashTable;
} MagicEntry_t;

void InitRookEntries(MagicEntry_t magicEntries[NUM_SQUARES]);

#endif