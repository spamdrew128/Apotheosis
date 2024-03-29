#ifdef __x86_64__
#include <immintrin.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "bitboards.h"

Bitboard_t NortOne (Bitboard_t b) {return  b << 8;}
Bitboard_t NoEaOne (Bitboard_t b) {return (b & NOT_H_FILE) << 9;}
Bitboard_t EastOne (Bitboard_t b) {return (b & NOT_H_FILE) << 1;}
Bitboard_t SoEaOne (Bitboard_t b) {return (b & NOT_H_FILE) >> 7;}
Bitboard_t SoutOne (Bitboard_t b) {return  b >> 8;}
Bitboard_t SoWeOne (Bitboard_t b) {return (b & NOT_A_FILE) >> 9;}
Bitboard_t WestOne (Bitboard_t b) {return (b & NOT_A_FILE) >> 1;}
Bitboard_t NoWeOne (Bitboard_t b) {return (b & NOT_A_FILE) << 7;}

Bitboard_t NortTwo(Bitboard_t b) {return  b << 16;}
Bitboard_t SoutTwo(Bitboard_t b) {return  b >> 16;}

Bitboard_t GenShiftNorth(Bitboard_t b, uint8_t shift) {
    return  b << (8 * shift);
}

Bitboard_t GenShiftSouth(Bitboard_t b, uint8_t shift) {
    return b >> (8 * shift);
}

Bitboard_t GenShiftEast(Bitboard_t b, uint8_t shift) {
    return b << shift;
}

Bitboard_t GenShiftWest(Bitboard_t b, uint8_t shift) {
    return b >> shift;
}

Population_t PopCount(Bitboard_t mask)
{
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)

    return (uint8_t)_mm_popcnt_u64(mask);

#else // Assumed gcc or compatible compiler

    return __builtin_popcountll(mask);

#endif
}

Square_t LSB(Bitboard_t b) {
    assert(b);
    return __builtin_ctzll(b);
}

Bitboard_t IsolateLSB(Bitboard_t b) {
    return b & -b;
}

void ResetLSB(Bitboard_t* b) {
    *b &= *b - 1;
}

void SetBits(Bitboard_t* b, Bitboard_t mask) {
    *b |= mask;
}

void ResetBits(Bitboard_t* b, Bitboard_t mask) {
    *b &= ~mask;
}

void ToggleBits(Bitboard_t* b, Bitboard_t mask) {
    *b ^= mask;
}

