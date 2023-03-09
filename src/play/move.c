#include "move.h"

enum offsets {
  from_square_offset = 6,
  promotion_piece_offset = 12
};

enum moveBitfields{
  to_square       = 0b0000000000111111,
  from_square     = 0b0000111111000000,
  promotion_piece = 0b0011000000000000,
  special_flags   = 0b1100000000000000
};

void InitMove(Move_t* move) {
    move->data = 0;
}

Move_t NullMove() {
    Move_t move = { .data = 0 };
    return move;
}

Square_t ReadToSquare(Move_t move) {
    return move.data & to_square;
}

Square_t ReadFromSquare(Move_t move) {
    return (move.data & from_square) >> from_square_offset;
}

Square_t ReadPromotionPiece(Move_t move) {
    return (move.data & promotion_piece) >> promotion_piece_offset;
}

SpecialFlag_t ReadSpecialFlag(Move_t move) {
    return move.data & special_flags;
}

void WriteToSquare(Move_t* move, Square_t square) {
    move->data |= square;
}

void WriteFromSquare(Move_t* move, Square_t square) {
    move->data |= (square << from_square_offset);
}

void WritePromotionPiece(Move_t* move, Piece_t piece) {
    move->data |= (piece << promotion_piece_offset);
}

void WriteSpecialFlag(Move_t* move, SpecialFlag_t flag) {
    move->data |= flag;
}

bool CompareMoves(Move_t m1, Move_t m2) {
    return m1.data == m2.data;
}