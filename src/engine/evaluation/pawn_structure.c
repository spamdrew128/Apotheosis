#include "pawn_structure.h"
#include "eval_constants.h"
#include "bitboards.h"
#include "eval_helpers.h"
#include "util_macros.h"

static Centipawns_t passerBonus[NUM_PHASES][NUM_SQUARES] = { { PASSED_PAWN_MG_PST }, { PASSED_PAWN_EG_PST } };
static Centipawns_t blockedPasserPenalty[NUM_PHASES][NUM_RANKS] = { { BLOCKED_PASSERS_MG }, { BLOCKED_PASSERS_EG } };

static Centipawns_t rookOpenBonus[NUM_PHASES][NUM_FILES] = { { ROOK_OPEN_FILE_MG }, { ROOK_OPEN_FILE_EG } };
static Centipawns_t rookSemiOpenBonus[NUM_PHASES][NUM_FILES] = { { ROOK_SEMI_OPEN_FILE_MG }, { ROOK_SEMI_OPEN_FILE_EG } };

static void PassedPawns(
    BoardInfo_t* boardInfo,
    Centipawns_t* mgScore,
    Centipawns_t* egScore
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

    SerializeBySquare(wPassers, bPassers, mgScore, egScore, passerBonus);

    SerializeByRank(piecesBlockingWhite, piecesBlockingBlack, mgScore, egScore, blockedPasserPenalty);
}

static void RookOpenFile(
    BoardInfo_t* boardInfo,
    Centipawns_t* mgScore,
    Centipawns_t* egScore
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

    SerializeByFile(whiteOpenRooks, blackOpenRooks, mgScore, egScore, rookOpenBonus);
    SerializeByFile(whiteSemiOpenRooks, blackSemiOpenRooks, mgScore, egScore, rookSemiOpenBonus);
}

void PawnStructureEval(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore) {
    PassedPawns(boardInfo, mgScore, egScore);
    RookOpenFile(boardInfo, mgScore, egScore); // it involves pawns to define open files, sue me.
}