#include "evaluation.h"
#include "PST.h"
#include "util_macros.h"

typedef uint8_t PieceCount_t;
typedef int32_t Centipawns_t;

static Centipawns_t midgamePST[6][NUM_SQUARES] = { KNIGHT_MG_PST, BISHOP_MG_PST, ROOK_MG_PST, QUEEN_MG_PST, PAWN_MG_PST, KING_MG_PST };
static Centipawns_t endgamePST[6][NUM_SQUARES] = { KNIGHT_EG_PST, BISHOP_EG_PST, ROOK_EG_PST, QUEEN_EG_PST, PAWN_EG_PST, KING_EG_PST };
static Phase_t gamePhaseLookup[6] = GAMEPHASE_VALUES;

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

    Phase_t mgPhase = (gamePhase < PHASE_MAX) ? gamePhase : PHASE_MAX;
    Phase_t egPhase = PHASE_MAX - mgPhase;
    return (mgScore * mgPhase + egScore * egPhase) / PHASE_MAX; // weighted average
}

EvalScore_t ScoreOfPosition(BoardInfo_t* boardInfo) {
    EvalScore_t eval = MaterialBalanceAndPSTBonus(boardInfo);

    return boardInfo->colorToMove == white ? eval : -eval;
}