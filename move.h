#ifndef __MOVE_H__
#define __MOVE_H__

#include <stdint.h>
#include "board_constants.h"

typedef struct {
    uint16_t data;
} Move_t;

enum offsets {
  from_square_offset = 6,
  promotion_piece_offset = 12,
  special_flag_offset = 14
};

enum moveBitfields{
  to_square       = 0b0000000000111111,
  from_square     = 0b0000111111000000,
  promotion_piece = 0b0011000000000000,
  special_flags   = 0b1100000000000000
};

typedef uint16_t SpecialFlag_t;
enum specialFlags {
  no_flag         = 0b00 << special_flag_offset,
  en_passant_flag = 0b01 << special_flag_offset,
  promotion_flag  = 0b10 << special_flag_offset,
  castle_flag     = 0b11 << special_flag_offset
};

#define InitalizeMove(move) move.data = 0

#define ReadToSquare(move) move.data & to_square
#define ReadFromSquare(move) (move.data & from_square) >> from_square_offset
#define ReadPromotionPiece(move) (move.data & promotion_piece) >> promotion_piece_offset
#define ReadSpecialFlag(move) move.data & special_flags

#define WriteToSquare(move, square) move.data |= square
#define WriteFromSquare(move, square) move.data |= (square << from_square_offset)
#define WritePromotionPiece(move, piece) move.data |= (piece << promotion_piece_offset)
#define WriteSpecialFlag(move, specialFlag) move.data |= specialFlag

#endif