#ifndef __BITBOARDS_H__
#define __BITBOARDS_H__

#include "board_constants.h"

Bitboard_t NortOne (Bitboard_t b);
Bitboard_t NoEaOne (Bitboard_t b);
Bitboard_t EastOne (Bitboard_t b);
Bitboard_t SoEaOne (Bitboard_t b);
Bitboard_t SoutOne (Bitboard_t b);
Bitboard_t SoWeOne (Bitboard_t b);
Bitboard_t WestOne (Bitboard_t b);
Bitboard_t NoWeOne (Bitboard_t b);

typedef int Population_t;
Population_t PopulationCount(Bitboard_t b);

Square_t LSB(Bitboard_t b);

#define ResetLSB(b) b &= b - 1
#define IsolateLSB(b) b & -b

#define SetBits(bitboard, mask) bitboard |= mask
#define ResetBits(bitboard, mask) bitboard &= ~mask
#define ToggleBits(bitboard, mask) bitboard ^= mask

#endif
