#ifndef __BITBOARDS_H__
#define __BITBOARDS_H__

#include <stdint.h>

#include "board_constants.h"

typedef Bitboard_t (*DirectionCallback_t)(Bitboard_t b); // putting this here cuz I use it everywhere

Bitboard_t NortOne (Bitboard_t b);
Bitboard_t NoEaOne (Bitboard_t b);
Bitboard_t EastOne (Bitboard_t b);
Bitboard_t SoEaOne (Bitboard_t b);
Bitboard_t SoutOne (Bitboard_t b);
Bitboard_t SoWeOne (Bitboard_t b);
Bitboard_t WestOne (Bitboard_t b);
Bitboard_t NoWeOne (Bitboard_t b);

Bitboard_t NortTwo(Bitboard_t b);
Bitboard_t SoutTwo(Bitboard_t b);

Bitboard_t GenShiftEast(Bitboard_t b, uint8_t shift);
Bitboard_t GenShiftWest(Bitboard_t b, uint8_t shift);

typedef uint8_t Population_t;
Population_t PopulationCount(Bitboard_t b);

Square_t LSB(Bitboard_t b);
Bitboard_t IsolateLSB(Bitboard_t b);

void ResetLSB(Bitboard_t* b);
void SetBits(Bitboard_t* b, Bitboard_t mask);
void ResetBits(Bitboard_t* b, Bitboard_t mask);
void ToggleBits(Bitboard_t* b, Bitboard_t mask);

#endif
