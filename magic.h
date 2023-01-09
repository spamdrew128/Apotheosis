#ifndef __MAGIC_H__
#define __MAGIC_H__

#include <stdint.h>

#include "board_constants.h"

typedef struct {
    Bitboard_t bitboard;
    Bitboard_t magic;
    uint8_t indexBits;
    const Bitboard_t* hashTable;
} MagicEntry_t;

#endif