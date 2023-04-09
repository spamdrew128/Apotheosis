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

enum squareSets {
  empty_set                 = C64(0),
  full_set                  = C64(0xffffffffffffffff),
  a_file                    = C64(0x0101010101010101),
  d_file                    = C64(0x0808080808080808),
  g_file                    = C64(0x4040404040404040),
  h_file                    = C64(0x8080808080808080),
  not_a_file                = C64(0xfefefefefefefefe), // ~a_file
  not_h_file                = C64(0x7f7f7f7f7f7f7f7f), // ~h_file
  not_ab_files              = C64(0xfcfcfcfcfcfcfcfc),
  not_gh_files              = C64(0x3f3f3f3f3f3f3f3f),
  rank_1                    = C64(0x00000000000000ff),
  rank_2                    = C64(0x000000000000ff00),
  rank_3                    = C64(0x0000000000ff0000),
  rank_4                    = C64(0x00000000ff000000),
  rank_5                    = C64(0x000000ff00000000),
  rank_6                    = C64(0x0000ff0000000000),
  rank_7                    = C64(0x00ff000000000000),
  rank_8                    = C64(0xff00000000000000),
  a1_h8_diagonal            = C64(0x8040201008040201),
  h1_a8_diagonal            = C64(0x0102040810204080),
  light_squares             = C64(0x55aa55aa55aa55aa),
  dark_squares              = C64(0xaa55aa55aa55aa55),
  board_corners             = C64(0x8100000000000081),
  white_queenside_castle_bb = C64(1) << c1,
  white_kingside_castle_bb  = C64(1) << g1,
  black_queenside_castle_bb = C64(1) << c8,
  black_kingside_castle_bb  = C64(1) << g8,
  w_qsc_vulnerable_squares  = C64(0x0c),
  w_ksc_vulnerable_squares  = C64(0x60),
  b_qsc_vulnerable_squares  = C64(0x0c) << 56,
  b_ksc_vulnerable_squares  = C64(0x60) << 56,
  w_qsc_blockable_squares   = C64(0x0e),
  w_ksc_blockable_squares   = C64(0x60),
  b_qsc_blockable_squares   = C64(0x0e) << 56,
  b_ksc_blockable_squares   = C64(0x60) << 56
};

#endif
