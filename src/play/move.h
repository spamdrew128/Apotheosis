#ifndef __MOVE_H__
#define __MOVE_H__

#include <stdint.h>
#include "board_constants.h"

typedef struct {
    uint16_t data;
} Move_t;

typedef uint16_t SpecialFlag_t;
enum specialFlags {
  no_flag         = 0b00 << 14,
  en_passant_flag = 0b01 << 14,
  promotion_flag  = 0b10 << 14,
  castle_flag     = 0b11 << 14
};

void InitMove(Move_t* move);

Move_t NullMove();

Square_t ReadToSquare(Move_t move);
Square_t ReadFromSquare(Move_t move);
Square_t ReadPromotionPiece(Move_t move);
SpecialFlag_t ReadSpecialFlag(Move_t move);

void WriteToSquare(Move_t* move, Square_t square);
void WriteFromSquare(Move_t* move, Square_t square);
void WritePromotionPiece(Move_t* move, Piece_t piece);
void WriteSpecialFlag(Move_t* move, SpecialFlag_t flag);

bool CompareMoves(Move_t m1, Move_t m2);

#endif