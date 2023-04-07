#include "pawn_structure.h"
#include "eval_constants.h"
#include "bitboards.h"
#include "eval_helpers.h"
#include "util_macros.h"

static Centipawns_t passerBonus[NUM_PHASES][NUM_SQUARES] = PASSED_PAWN_PST;
static Centipawns_t blockedPasserPenalty[NUM_PHASES][NUM_RANKS] = BLOCKED_PASSERS;

static Centipawns_t rookOpenBonus[NUM_PHASES][NUM_FILES] = ROOK_OPEN_FILE;
static Centipawns_t rookSemiOpenBonus[NUM_PHASES][NUM_FILES] = ROOK_SEMI_OPEN_FILE;
static Centipawns_t queenOpenBonus[NUM_PHASES][NUM_FILES] = QUEEN_OPEN_FILE;
static Centipawns_t queenSemiOpenBonus[NUM_PHASES][NUM_FILES] = QUEEN_SEMI_OPEN_FILE;
static Centipawns_t kingOpenBonus[NUM_PHASES][NUM_FILES] = KING_OPEN_FILE;
static Centipawns_t kingSemiOpenBonus[NUM_PHASES][NUM_FILES] = KING_SEMI_OPEN_FILE;

void PassedPawnBonus(
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

void OpenFileBonus(
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

    Bitboard_t whiteOpenQueens = boardInfo->queens[white] & openFiles;
    Bitboard_t blackOpenQueens = boardInfo->queens[black] & openFiles;

    Bitboard_t whiteSemiOpenQueens = boardInfo->queens[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenQueens = boardInfo->queens[black] & whitePawnOnlyFiles;

    SerializeByFile(whiteOpenQueens, blackOpenQueens, mgScore, egScore, queenOpenBonus);
    SerializeByFile(whiteSemiOpenQueens, blackSemiOpenQueens, mgScore, egScore, queenSemiOpenBonus);

    Bitboard_t whiteOpenKings = boardInfo->kings[white] & openFiles;
    Bitboard_t blackOpenKings = boardInfo->kings[black] & openFiles;

    Bitboard_t whiteSemiOpenKings = boardInfo->kings[white] & blackPawnOnlyFiles;
    Bitboard_t blackSemiOpenKings = boardInfo->kings[black] & whitePawnOnlyFiles;

    SerializeByFile(whiteOpenKings, blackOpenKings, mgScore, egScore, kingOpenBonus);
    SerializeByFile(whiteSemiOpenKings, blackSemiOpenKings, mgScore, egScore, kingSemiOpenBonus);
}