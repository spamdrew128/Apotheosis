#include "pawn_structure.h"
#include "eval_constants.h"
#include "bitboards.h"
#include "eval_helpers.h"
#include "util_macros.h"

static Score_t passerBonus[NUM_PST_BUCKETS][NUM_SQUARES] = PASSED_PAWN_PST;
static Score_t blockedPasserPenalty[NUM_RANKS] = BLOCKED_PASSERS;

static Score_t rookOpenBonus[NUM_FILES] = ROOK_OPEN_FILE;
static Score_t rookSemiOpenBonus[NUM_FILES] = ROOK_SEMI_OPEN_FILE;
static Score_t queenOpenBonus[NUM_FILES] = QUEEN_OPEN_FILE;
static Score_t queenSemiOpenBonus[NUM_FILES] = QUEEN_SEMI_OPEN_FILE;
static Score_t kingOpenBonus[NUM_FILES] = KING_OPEN_FILE;
static Score_t kingSemiOpenBonus[NUM_FILES] = KING_SEMI_OPEN_FILE;

static Score_t isolatedPawnPenalty[NUM_FILES] = ISOLATED_PAWNS;
static Score_t doubledPawnPenalty[NUM_FILES] = DOUBLED_PAWNS;

void PassedPawnBonus(
    BoardInfo_t* boardInfo,
    const Bucket_t wBucket,
    const Bucket_t bBucket,
    Score_t* score
)
{
    const Bitboard_t wFrontSpan = WhiteForwardFill(boardInfo->pawns[white]);
    const Bitboard_t bFrontSpan = BlackForwardFill(boardInfo->pawns[black]);

    const Bitboard_t wPawnBlocks = wFrontSpan | EastOne(wFrontSpan) | WestOne(wFrontSpan);
    const Bitboard_t bPawnBlocks = bFrontSpan | EastOne(bFrontSpan) | WestOne(bFrontSpan);

    Bitboard_t wPassers = boardInfo->pawns[white] & ~bPawnBlocks;
    Bitboard_t bPassers = boardInfo->pawns[black] & ~wPawnBlocks;

    Bitboard_t piecesBlockingWhite = NortOne(wPassers) & boardInfo->allPieces[black];
    Bitboard_t piecesBlockingBlack = SoutOne(bPassers) & boardInfo->allPieces[white];

    SerializeBySquare(wPassers, bPassers, wBucket, bBucket, score, passerBonus);

    SerializeByRank(piecesBlockingWhite, piecesBlockingBlack, score, blockedPasserPenalty);
}

void PawnStructure(
    BoardInfo_t* boardInfo,
    Score_t* score
)
{
    const Bitboard_t whiteFill = FileFill(boardInfo->pawns[white]);
    const Bitboard_t blackFill = FileFill(boardInfo->pawns[black]);

    const Bitboard_t whiteNeighbors = EastOne(whiteFill) | WestOne(whiteFill);
    const Bitboard_t blackNeighbors = EastOne(blackFill) | WestOne(blackFill);

    Bitboard_t wIsolated = boardInfo->pawns[white] & ~whiteNeighbors;
    Bitboard_t bIsolated = boardInfo->pawns[black] & ~blackNeighbors;

    SerializeByFile(wIsolated, bIsolated, score, isolatedPawnPenalty);

    const Bitboard_t wFrontSpan = WhiteForwardFill(boardInfo->pawns[white]);
    const Bitboard_t bFrontSpan = BlackForwardFill(boardInfo->pawns[black]);

    const Bitboard_t wDoubled = boardInfo->pawns[white] & wFrontSpan;
    const Bitboard_t bDoubled = boardInfo->pawns[black] & bFrontSpan;

    SerializeByFile(wDoubled, bDoubled, score, doubledPawnPenalty);
}

void OpenFileBonus(
    BoardInfo_t* boardInfo,
    Score_t* score
)
{
    const Bitboard_t whitePawnFileSpans = FileFill(boardInfo->pawns[white]);
    const Bitboard_t blackPawnFileSpans = FileFill(boardInfo->pawns[black]);

    const Bitboard_t openFiles = ~(whitePawnFileSpans | blackPawnFileSpans);
    const Bitboard_t whitePawnOnlyFiles = whitePawnFileSpans & ~blackPawnFileSpans;
    const Bitboard_t blackPawnOnlyFiles = blackPawnFileSpans & ~whitePawnFileSpans;

    Bitboard_t whiteOpenRooks = boardInfo->rooks[white] & openFiles;
    Bitboard_t blackOpenRooks = boardInfo->rooks[black] & openFiles;

    Bitboard_t whiteSemiOpenRooks = boardInfo->rooks[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenRooks = boardInfo->rooks[black] & whitePawnOnlyFiles;

    SerializeByFile(whiteOpenRooks, blackOpenRooks, score, rookOpenBonus);
    SerializeByFile(whiteSemiOpenRooks, blackSemiOpenRooks, score, rookSemiOpenBonus);

    Bitboard_t whiteOpenQueens = boardInfo->queens[white] & openFiles;
    Bitboard_t blackOpenQueens = boardInfo->queens[black] & openFiles;

    Bitboard_t whiteSemiOpenQueens = boardInfo->queens[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenQueens = boardInfo->queens[black] & whitePawnOnlyFiles;

    SerializeByFile(whiteOpenQueens, blackOpenQueens, score, queenOpenBonus);
    SerializeByFile(whiteSemiOpenQueens, blackSemiOpenQueens, score, queenSemiOpenBonus);

    Bitboard_t whiteOpenKings = boardInfo->kings[white] & openFiles;
    Bitboard_t blackOpenKings = boardInfo->kings[black] & openFiles;

    Bitboard_t whiteSemiOpenKings = boardInfo->kings[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenKings = boardInfo->kings[black] & whitePawnOnlyFiles;

    SerializeByFile(whiteOpenKings, blackOpenKings, score, kingOpenBonus);
    SerializeByFile(whiteSemiOpenKings, blackSemiOpenKings, score, kingSemiOpenBonus);
}