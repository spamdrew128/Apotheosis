#ifndef __BOARD_CONSTANTS_H__
#define __BOARD_CONSTANTS_H__

typedef unsigned long long Bitboard_t; // supported by MSC 13.00+ and C99 
#define C64(constantU64) constantU64##ULL

#define NUM_SQUARES 64
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"

typedef int Color_t;
enum side {
    white,
    black
};

typedef int Square_t;
enum squareLabels {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
};

enum squareSets {
  a_file         = 0x0101010101010101,
  h_file         = 0x8080808080808080,
  not_a_file     = 0xfefefefefefefefe, // ~a_file
  not_h_file     = 0x7f7f7f7f7f7f7f7f, // ~h_file
  not_ab_files   = 0xFCFCFCFCFCFCFCFC,
  not_gh_files   = 0x3F3F3F3F3F3F3F3F,
  first_rank     = 0x00000000000000FF,
  second_rank    = 0x000000000000FF00,
  third_rank     = 0x0000000000FF0000,
  fourth_rank    = 0x00000000FF000000,
  fifth_rank     = 0x000000FF00000000,
  sixth_rank     = 0x0000FF0000000000,
  seventh_rank   = 0x00FF000000000000,
  eighth_rank    = 0xFF00000000000000,
  a1_h8_diagonal = 0x8040201008040201,
  h1_a8_diagonal = 0x0102040810204080,
  light_squares  = 0x55AA55AA55AA55AA,
  dark_squares   = 0xAA55AA55AA55AA55
};

#endif
