#include "attack_eval.h"
#include "lookup.h"

static Score_t knightMobility[KNIGHT_MOBILITY_OPTIONS] = {0};
static Score_t bishopMobility[BISHOP_MOBILITY_OPTIONS] = {0};
static Score_t rookMobility[ROOK_MOBILITY_OPTIONS] = {0};
static Score_t queenMobility[QUEEN_MOBILITY_OPTIONS] = {0};
static Score_t kingMobility[KING_MOBILITY_OPTIONS] = {0};

void MobilityEval(BoardInfo_t* boardInfo, Score_t* score) {

}