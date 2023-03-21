#include <assert.h>

#include "evaluation.h"
#include "PST.h"
#include "util_macros.h"
#include "legals.h"

enum {
    mobility_weight = 5
};

// eventually I'll have different piece values for midgame and endgame.
// also I needed to make none_type worth as much as a pawn so I don't need an edge case for en_passant. Really hacky but it works lol.
static Centipawns_t pieceValues[7] = { knight_value, bishop_value, rook_value, queen_value, pawn_value, king_value, pawn_value };

static Centipawns_t midgamePST[6][NUM_SQUARES] = { KNIGHT_MG_PST, BISHOP_MG_PST, ROOK_MG_PST, QUEEN_MG_PST, PAWN_MG_PST, KING_MG_PST };
static Centipawns_t endgamePST[6][NUM_SQUARES] = { KNIGHT_EG_PST, BISHOP_EG_PST, ROOK_EG_PST, QUEEN_EG_PST, PAWN_EG_PST, KING_EG_PST };
static Phase_t gamePhaseLookup[6] = GAMEPHASE_VALUES;

Centipawns_t ValueOfPiece(Piece_t piece) {
    return pieceValues[piece];
}

static void MaterialAndPST(
    Bitboard_t infoField[2],
    Piece_t piece,
    Centipawns_t value,
    Phase_t* gamePhase,
    Centipawns_t* mgScore,
    Centipawns_t* egScore
)
{
    Bitboard_t whitePieces = infoField[white];
    Bitboard_t blackPieces = infoField[black];

    while(whitePieces) {
        Square_t sq = MIRROR(LSB(whitePieces));
        *mgScore += value + midgamePST[piece][sq];
        *egScore += value + endgamePST[piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&whitePieces);
    }
    while(blackPieces) {
        Square_t sq = LSB(blackPieces);
        *mgScore -= value + midgamePST[piece][sq];
        *egScore -= value + endgamePST[piece][sq];
        *gamePhase += gamePhaseLookup[piece];
        ResetLSB(&blackPieces);
    }
}

static void AddKingPSTBonus(BoardInfo_t* boardInfo, Centipawns_t* mgScore, Centipawns_t* egScore) {
    Square_t whiteKing = MIRROR(KingSquare(boardInfo, white));
    Square_t blackKing = KingSquare(boardInfo, black);

    *mgScore += midgamePST[king][whiteKing];
    *egScore += endgamePST[king][whiteKing];

    *mgScore -= midgamePST[king][blackKing];
    *egScore -= endgamePST[king][blackKing];
}

static Centipawns_t MaterialBalanceAndPSTBonus(BoardInfo_t* boardInfo) {
    Centipawns_t mgScore = 0;
    Centipawns_t egScore = 0;
    Phase_t gamePhase = 0;

    MaterialAndPST(boardInfo->knights, knight, knight_value, &gamePhase, &mgScore, &egScore);
    MaterialAndPST(boardInfo->bishops, bishop, bishop_value, &gamePhase, &mgScore, &egScore);
    MaterialAndPST(boardInfo->rooks, rook, rook_value, &gamePhase, &mgScore, &egScore);
    MaterialAndPST(boardInfo->queens, queen, queen_value, &gamePhase, &mgScore, &egScore);
    MaterialAndPST(boardInfo->pawns, pawn, pawn_value, &gamePhase, &mgScore, &egScore);
    AddKingPSTBonus(boardInfo, &mgScore, &egScore);

    Phase_t mgPhase = MIN(gamePhase, PHASE_MAX);
    Phase_t egPhase = PHASE_MAX - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / PHASE_MAX; // weighted average
}

static Centipawns_t MobilityEval(BoardInfo_t* boardInfo) {
    Bitboard_t empty = boardInfo->empty;
    Bitboard_t whitePseudolegals = AttackedSquares(boardInfo, empty, white) & ~boardInfo->allPieces[white];
    Bitboard_t blackPseudolegals = AttackedSquares(boardInfo, empty, black) & ~boardInfo->allPieces[black];

    return (PopCount(whitePseudolegals) - PopCount(blackPseudolegals)) * mobility_weight;
}

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo) {
    EvalScore_t eval = 0;
    eval += MaterialBalanceAndPSTBonus(boardInfo);
    eval += MobilityEval(boardInfo);

    return boardInfo->colorToMove == white ? eval : -eval;
}