#include <assert.h>

#include "lookup.h"
#include "bitboards.h"

static Lookup_t lookup;

typedef Bitboard_t (*DirectionCallback_t)(Bitboard_t);

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

static void InitKnightAttacks(Bitboard_t knightAttacks[]) {
    for(Square_t i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t squareBitset = GetSingleBitset(i);
        knightAttacks[i] = 
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

static void InitKingAttacks(Bitboard_t kingAttacks[]) {
    for(Square_t i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t squareBitset = GetSingleBitset(i);
        kingAttacks[i] = 
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

static void InitPawnAttacks(Bitboard_t pawnAttacks[][NUM_SQUARES]) {
    for(Square_t i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t squareBitset = GetSingleBitset(i);
        pawnAttacks[white][i] = 
            NoEaOne(squareBitset) |
            NoWeOne(squareBitset);  

        pawnAttacks[black][i] =
            SoEaOne(squareBitset) |
            SoWeOne(squareBitset);       
    }
}

static void InitializeSlidingCheckmasksWithZeros(Bitboard_t slidingCheckmasks[][NUM_SQUARES]) {
    for(int i = 0; i < NUM_SQUARES; i++) {
        for(int j = 0; j < NUM_SQUARES; j++) {
            slidingCheckmasks[i][j] = 0;
        }
    }
}

static void FillSlidingCheckmask(Bitboard_t* checkmasks, Bitboard_t singleBitset, DirectionCallback_t DirectionFunc) {
    Bitboard_t result = empty_set;

    singleBitset = DirectionFunc(singleBitset);
    while(singleBitset) {
        result |= singleBitset;
        checkmasks[LSB(singleBitset)] = result;
        singleBitset = DirectionFunc(singleBitset);
    }
}

static void InitSlidingCheckmasks(Bitboard_t slidingCheckmasks[][NUM_SQUARES]) {
    InitializeSlidingCheckmasksWithZeros(slidingCheckmasks);

    for(Square_t kingSquare = 0; kingSquare < NUM_SQUARES; kingSquare++) {
        Bitboard_t kingBitboard = GetSingleBitset(kingSquare);
        
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, NortOne);
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, EastOne);
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, SoutOne);
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, WestOne);
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, NoEaOne);
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, SoEaOne);
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, SoWeOne);
        FillSlidingCheckmask(slidingCheckmasks[kingSquare], kingBitboard, NoWeOne);
    }
}

void InitLookup() {
    InitSingleBitset(lookup.singleBitsets);
    InitKnightAttacks(lookup.knightAttacks);
    InitKingAttacks(lookup.kingAttacks);
    InitPawnAttacks(lookup.pawnAttacks);
    InitRookEntries(lookup.rookMagicEntries);
    InitBishopEntries(lookup.bishopMagicEntries);
    InitSlidingCheckmasks(lookup.slidingCheckmasks);
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

Bitboard_t GetPawnAttacks(Square_t square, Color_t color) {
    return lookup.pawnAttacks[color][square];
}

MagicEntry_t GetRookMagicEntry(Square_t square) {
    return lookup.rookMagicEntries[square];
}

MagicEntry_t GetBishopMagicEntry(Square_t square) {
    return lookup.bishopMagicEntries[square];
}

Bitboard_t GetSlidingCheckmasks(Square_t kingSquare, Square_t slidingPieceSquare) {
    Bitboard_t checkmask = lookup.slidingCheckmasks[kingSquare][slidingPieceSquare];
    assert(checkmask);
    return checkmask;
}

void TeardownLookup() {
    FreeMagicEntries(lookup.rookMagicEntries);
    FreeMagicEntries(lookup.bishopMagicEntries);
}