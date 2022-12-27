#ifndef __BOARD_CONSTANTS_H__
#define __BOARD_CONSTANTS_H__

typedef unsigned long long  U64; // supported by MSC 13.00+ and C99 
#define C64(constantU64) constantU64##ULL

enum squareLabels{
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
};

enum squareSets{
  a_file         = 0x0101010101010101,
  h_file         = 0x8080808080808080,
  not_a_file     = 0xfefefefefefefefe, // ~a_file
  not_h_file     = 0x7f7f7f7f7f7f7f7f, // ~h_file
  first_rank     = 0x00000000000000FF,
  eighth_rank    = 0xFF00000000000000,
  a1_h8_diagonal = 0x8040201008040201,
  h1_a8_diagonal = 0x0102040810204080,
  light_squares  = 0x55AA55AA55AA55AA,
  dark_squares   = 0xAA55AA55AA55AA55
};

#endif
