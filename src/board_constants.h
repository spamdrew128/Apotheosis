#ifndef __BOARD_CONSTANTS_H__
#define __BOARD_CONSTANTS_H__

#include <stdint.h>

typedef uint64_t Bitboard_t; // supported by MSC 13.00+ and C99 
#define C64(constantU64) constantU64##ULL

#define NUM_PIECES 6
#define MOVELIST_MAX 256
#define GAMESTATES_MAX 1024
#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#include <stdbool.h>
#include <stdint.h>

typedef bool Color_t;
enum color {
  white,
  black
};

typedef uint8_t Rank_t;
#define NUM_RANKS 8

typedef uint8_t File_t;
#define NUM_FILES 8

typedef uint8_t Square_t;
enum squareLabels {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8,
  NUM_SQUARES
};

typedef uint8_t Piece_t;
// ORDERING IS EXTREMELY IMPORTANT
// CHANGING ORDER WILL F UP MOVE.H
enum pieceTypes {
  knight,
  bishop,
  rook,
  queen,
  pawn,
  king,
  none_type,
};

typedef uint8_t Direction_t;
enum directions {
  N,
  NE,
  E,
  SE,
  S,
  SW,
  W,
  NW,
  NUM_DIRECTIONS
};

// Bitboard Constants (originally and enum, but I had to change because some people had problems with size)
#define EMPTY_SET                  C64(0)
#define FULL_SET                   C64(0xffffffffffffffff)
#define A_FILE                     C64(0x0101010101010101)
#define D_FILE                     C64(0x0808080808080808)
#define H_FILE                     C64(0x8080808080808080)
#define NOT_A_FILE                 C64(0xfefefefefefefefe) // ~A_FILE
#define NOT_H_FILE                 C64(0x7f7f7f7f7f7f7f7f) // ~H_FILE
#define NOT_AB_FILES               C64(0xfcfcfcfcfcfcfcfc)
#define NOT_GH_FILES               C64(0x3f3f3f3f3f3f3f3f)
#define RANK_1                     C64(0x00000000000000ff)
#define RANK_2                     C64(0x000000000000ff00)
#define RANK_3                     C64(0x0000000000ff0000)
#define RANK_4                     C64(0x00000000ff000000)
#define RANK_5                     C64(0x000000ff00000000)
#define RANK_6                     C64(0x0000ff0000000000)
#define RANK_7                     C64(0x00ff000000000000)
#define RANK_8                     C64(0xff00000000000000)
#define A1_H8_DIAGONAL             C64(0x8040201008040201)
#define H1_A8_DIAGONAL             C64(0x0102040810204080)
#define BOARD_CORNERS              C64(0x8100000000000081)
#define WHITE_QUEENSIDE_CASTLE_BB  (C64(1) << c1)
#define WHITE_KINGSIDE_CASTLE_BB   (C64(1) << g1)
#define BLACK_QUEENSIDE_CASTLE_BB  (C64(1) << c8)
#define BLACK_KINGSIDE_CASTLE_BB   (C64(1) << g8)
#define W_QSC_VULNERABLE_SQUARES   C64(0x0c)
#define W_KSC_VULNERABLE_SQUARES   C64(0x60)
#define B_QSC_VULNERABLE_SQUARES   (C64(0x0c) << 56)
#define B_KSC_VULNERABLE_SQUARES   (C64(0x60) << 56)
#define W_QSC_BLOCKABLE_SQUARES    C64(0x0e)
#define W_KSC_BLOCKABLE_SQUARES    C64(0x60)
#define B_QSC_BLOCKABLE_SQUARES    (C64(0x0e) << 56)
#define B_KSC_BLOCKABLE_SQUARES    (C64(0x60) << 56)

#endif
