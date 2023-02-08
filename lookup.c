#include <assert.h>

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
        SetBits(&result, singleBitset);
        checkmasks[LSB(singleBitset)] = result;
        singleBitset = DirectionFunc(singleBitset);
    }
}

static void InitPawnCheckmasks(Bitboard_t pawnCheckmasks[][NUM_SQUARES]) {
    for(Square_t i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t squareBitset = GetSingleBitset(i);
        
        pawnCheckmasks[white][i] =
            NoEaOne(squareBitset) |
            NoWeOne(squareBitset);  

        pawnCheckmasks[black][i] = 
            SoEaOne(squareBitset) |
            SoWeOne(squareBitset); 
    }
}

static void InitSlidingCheckmasks(Bitboard_t slidingCheckmasks[NUM_SQUARES][NUM_SQUARES]) {
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

static Bitboard_t GetSingleDirectionRay(Bitboard_t singleBitset, DirectionCallback_t DirectionFunc) {
    Bitboard_t result = empty_set;

    singleBitset = DirectionFunc(singleBitset);
    while(singleBitset) {
        SetBits(&result, singleBitset);
        singleBitset = DirectionFunc(singleBitset);
    }

    return result;
}

static void InitDirectionalRays(Bitboard_t directionalRays[NUM_SQUARES][NUM_DIRECTIONS]) {
    for(int i = 0; i < NUM_SQUARES; i++) {
        Bitboard_t singleBitset = GetSingleBitset(i);

        directionalRays[i][N] = GetSingleDirectionRay(singleBitset, NortOne);
        directionalRays[i][NE] = GetSingleDirectionRay(singleBitset, NoEaOne);
        directionalRays[i][E] = GetSingleDirectionRay(singleBitset, EastOne);
        directionalRays[i][SE] = GetSingleDirectionRay(singleBitset, SoEaOne);
        directionalRays[i][S] = GetSingleDirectionRay(singleBitset, SoutOne);
        directionalRays[i][SW] = GetSingleDirectionRay(singleBitset, SoWeOne);
        directionalRays[i][W] = GetSingleDirectionRay(singleBitset, WestOne);
        directionalRays[i][NW] = GetSingleDirectionRay(singleBitset, NoWeOne);
    }
}

static void InitCastleSquares(Square_t ksCastleSquares[], Square_t qsCastleSquares[]) {
    ksCastleSquares[white] = LSB(white_kingside_castle_bb);
    ksCastleSquares[black] = LSB(black_kingside_castle_bb);

    qsCastleSquares[white] = LSB(white_queenside_castle_bb);
    qsCastleSquares[black] = LSB(black_queenside_castle_bb);
}

void InitLookup() {
    InitSingleBitset(lookup.singleBitsets);
    InitKnightAttacks(lookup.knightAttacks);
    InitKingAttacks(lookup.kingAttacks);
    InitAllMagicEntries(lookup.rookMagicEntries, lookup.bishopMagicEntries, lookup.magicHashTable);
    InitSlidingCheckmasks(lookup.slidingCheckmasks);
    InitPawnCheckmasks(lookup.pawnCheckmasks);
    InitDirectionalRays(lookup.directionalRays);
    InitCastleSquares(lookup.ksCastleSquares, lookup.qsCastleSquares);
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

MagicEntry_t GetRookMagicEntry(Square_t square) {
    return lookup.rookMagicEntries[square];
}

MagicEntry_t GetBishopMagicEntry(Square_t square) {
    return lookup.bishopMagicEntries[square];
}

Bitboard_t GetSlidingCheckmask(Square_t kingSquare, Square_t slidingPieceSquare) {
    Bitboard_t checkmask = lookup.slidingCheckmasks[kingSquare][slidingPieceSquare];
    assert(checkmask);
    return checkmask;
}

Bitboard_t GetPawnCheckmask(Square_t kingSquare, Color_t color) { // might be redundant but I don't care
    return lookup.pawnCheckmasks[color][kingSquare];
}

Bitboard_t GetDirectionalRay(Square_t square, Direction_t direction) {
    return lookup.directionalRays[square][direction];
}

Square_t GetKingsideCastleSquare(Color_t color) {
    return lookup.ksCastleSquares[color];
}

Square_t GetQueensideCastleSquare(Color_t color) {
    return lookup.qsCastleSquares[color];
}