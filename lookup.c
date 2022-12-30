#include "lookup.h"
#include "bitboards.h"

static Lookup_t lookup;

static void InitSingleBitset(Bitboard_t singleBitsets[]) {
    for(Square_t i = 0; i < NUM_SQUARES; i++) {
        singleBitsets[i] = C64(1) << i;
    }
}

// these are knight moves
static Bitboard_t NoNoEa(Bitboard_t b) {return (b & not_h_file ) << 17;}
static Bitboard_t NoEaEa(Bitboard_t b) {return (b & not_gh_files) << 10;}
static Bitboard_t SoEaEa(Bitboard_t b) {return (b & not_gh_files) >>  6;}
static Bitboard_t SoSoEa(Bitboard_t b) {return (b & not_h_file ) >> 15;}
static Bitboard_t NoNoWe(Bitboard_t b) {return (b & not_a_file ) << 15;}
static Bitboard_t NoWeWe(Bitboard_t b) {return (b & not_ab_files) <<  6;}
static Bitboard_t SoWeWe(Bitboard_t b) {return (b & not_ab_files) >> 10;}
static Bitboard_t SoSoWe(Bitboard_t b) {return (b & not_a_file ) >> 17;}

static void InitKnightAttacks(Bitboard_t knightMoves[]) {
    for(Square_t i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t squareBitset = GetSingleBitset(i);
        knightMoves[i] = 
            NoNoEa(squareBitset) |
            NoEaEa(squareBitset) |
            SoEaEa(squareBitset) |
            SoSoEa(squareBitset) |
            NoNoWe(squareBitset) |
            NoWeWe(squareBitset) |
            SoWeWe(squareBitset) |
            SoSoWe(squareBitset);         
    }
}

static void InitKingAttacks(Bitboard_t kingMoves[]) {
    for(Square_t i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t squareBitset = GetSingleBitset(i);
        kingMoves[i] = 
            NortOne(squareBitset) |
            NoEaOne(squareBitset) |
            EastOne(squareBitset) |
            SoEaOne(squareBitset) |
            SoutOne(squareBitset) |
            SoWeOne(squareBitset) |
            WestOne(squareBitset) |
            NoWeOne(squareBitset);         
    }
}

void InitLookup() {
    InitSingleBitset(lookup.singleBitsets);
    InitKnightAttacks(lookup.knightAttacks);
    InitKingAttacks(lookup.kingAttacks);
}

Bitboard_t GetSingleBitset(Square_t square) {
    return lookup.singleBitsets[square];
}

Bitboard_t GetKnightAttacks(Square_t square) {
    return lookup.knightAttacks[square];
}

Bitboard_t GetKingAttacks(Square_t square) {
    return lookup.kingAttacks[square];
}