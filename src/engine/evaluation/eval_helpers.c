#include "eval_helpers.h"
#include "util_macros.h"

Bitboard_t WhiteFill(Bitboard_t b) {
    b |= GenShiftNorth(b, 1);
    b |= GenShiftNorth(b, 2);
    b |= GenShiftNorth(b, 4);
    return b;
}

Bitboard_t BlackFill(Bitboard_t b) {
    b |= GenShiftSouth(b, 1);
    b |= GenShiftSouth(b, 2);
    b |= GenShiftSouth(b, 4);
    return b;
}

Bitboard_t FileFill(Bitboard_t b) {
    return WhiteFill(BlackFill(b));
}

Bitboard_t WhiteForwardFill(Bitboard_t b) {
    return NortOne(WhiteFill(b));
}

Bitboard_t BlackForwardFill(Bitboard_t b) {
    return SoutOne(BlackFill(b));
}

void SerializeBySquare(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    const Bucket_t wBucket,
    const Bucket_t bBucket,
    Score_t* score,
    Score_t bonus[NUM_PST_BUCKETS][NUM_SQUARES]
)
{
    while(whitePieces) {
        Square_t sq = MIRROR(LSB(whitePieces));
        *score += bonus[wBucket][sq];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Square_t sq = LSB(blackPieces);
        *score -= bonus[bBucket][sq];
        ResetLSB(&blackPieces);
    }
}

void SerializeByFile(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    Score_t* score,
    Score_t bonus[NUM_FILES]
)
{
    while(whitePieces) {
        File_t file = LSB(whitePieces) % 8;
        *score += bonus[file];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        File_t file = LSB(blackPieces) % 8;
        *score -= bonus[file];
        ResetLSB(&blackPieces);
    }
}

void SerializeByRank(
    Bitboard_t whitePieces,
    Bitboard_t blackPieces,
    Score_t* score,
    Score_t bonus[NUM_RANKS]
)
{
    while(whitePieces) {
        Rank_t rank = (MIRROR(LSB(whitePieces))) / 8;
        *score += bonus[rank];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Rank_t rank = LSB(blackPieces) / 8;
        *score -= bonus[rank];
        ResetLSB(&blackPieces);
    }
}