#ifndef __EVAL_CONSTANTS_H__
#define __EVAL_CONSTANTS_H__

#include <stdint.h>

typedef uint8_t Phase_t;
enum {
  mg_phase,
  eg_phase,
  NUM_PHASES,
  PHASE_MAX = 24,

  KNIGHT_PHASE_VALUE = 1,
  BISHOP_PHASE_VALUE = 1,
  ROOK_PHASE_VALUE = 2,
  QUEEN_PHASE_VALUE = 4,
  PAWN_PHASE_VALUE = 0,
  KING_PHASE_VALUE = 0,
};

/*

██████╗ ██╗███████╗ ██████╗███████╗    ███████╗ ██████╗ ██╗   ██╗ █████╗ ██████╗ ███████╗    ████████╗ █████╗ ██████╗ ██╗     ███████╗███████╗
██╔══██╗██║██╔════╝██╔════╝██╔════╝    ██╔════╝██╔═══██╗██║   ██║██╔══██╗██╔══██╗██╔════╝    ╚══██╔══╝██╔══██╗██╔══██╗██║     ██╔════╝██╔════╝
██████╔╝██║█████╗  ██║     █████╗      ███████╗██║   ██║██║   ██║███████║██████╔╝█████╗         ██║   ███████║██████╔╝██║     █████╗  ███████╗
██╔═══╝ ██║██╔══╝  ██║     ██╔══╝      ╚════██║██║▄▄ ██║██║   ██║██╔══██║██╔══██╗██╔══╝         ██║   ██╔══██║██╔══██╗██║     ██╔══╝  ╚════██║
██║     ██║███████╗╚██████╗███████╗    ███████║╚██████╔╝╚██████╔╝██║  ██║██║  ██║███████╗       ██║   ██║  ██║██████╔╝███████╗███████╗███████║
╚═╝     ╚═╝╚══════╝ ╚═════╝╚══════╝    ╚══════╝ ╚══▀▀═╝  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝       ╚═╝   ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝╚══════╝

*/

// all values are from black's perspective, becuase my scheme is flipped

#define PAWN_MG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   150, 150, 150, 150, 150, 150, 150, 150, \
   110, 110, 120, 130, 130, 120, 110, 110, \
   100, 100, 110, 125, 125, 110, 100, 100, \
   100, 100, 100, 125, 125, 100, 100, 100, \
   100, 100, 90, 100, 100, 90, 105, 100, \
   105, 110, 110, 80, 80, 110, 110, 105, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define PAWN_EG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   250, 250, 250, 250, 250, 250, 250, 250, \
   200, 180, 170, 170, 170, 180, 200, 200, \
   130, 130, 125, 120, 120, 125, 130, 130, \
   100, 100, 100, 120, 120, 100, 100, 100, \
   105, 95, 90, 100, 100, 90, 95, 105, \
   105, 110, 110, 80, 80, 110, 110, 105, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define KNIGHT_MG_PST \
   260, 270, 280, 280, 280, 280, 270, 260, \
   270, 290, 310, 310, 310, 310, 290, 270, \
   280, 310, 320, 325, 325, 320, 310, 280, \
   280, 315, 325, 330, 330, 325, 315, 280, \
   280, 310, 325, 330, 330, 325, 310, 280, \
   280, 315, 320, 325, 325, 320, 315, 280, \
   270, 290, 310, 315, 315, 310, 290, 270, \
   260, 270, 280, 280, 280, 280, 270, 260, \

#define KNIGHT_EG_PST \
   260, 270, 280, 280, 280, 280, 270, 260, \
   270, 290, 310, 310, 310, 310, 290, 270, \
   280, 310, 320, 325, 325, 320, 310, 280, \
   280, 315, 325, 330, 330, 325, 315, 280, \
   280, 310, 325, 330, 330, 325, 310, 280, \
   280, 315, 320, 325, 325, 320, 315, 280, \
   270, 290, 310, 315, 315, 310, 290, 270, \
   260, 270, 280, 280, 280, 280, 270, 260, \

#define BISHOP_MG_PST \
   310, 320, 320, 320, 320, 320, 320, 310, \
   320, 330, 330, 330, 330, 330, 330, 320, \
   320, 330, 335, 340, 340, 335, 330, 320, \
   320, 335, 335, 340, 340, 335, 335, 320, \
   320, 330, 340, 340, 340, 340, 330, 320, \
   320, 335, 340, 340, 340, 340, 335, 320, \
   320, 340, 330, 330, 330, 330, 340, 320, \
   310, 320, 320, 320, 320, 320, 320, 310, \

#define BISHOP_EG_PST \
   310, 320, 320, 320, 320, 320, 320, 310, \
   320, 330, 330, 330, 330, 330, 330, 320, \
   320, 330, 335, 340, 340, 335, 330, 320, \
   320, 335, 335, 340, 340, 335, 335, 320, \
   320, 330, 340, 340, 340, 340, 330, 320, \
   320, 340, 340, 340, 340, 340, 340, 320, \
   320, 335, 330, 330, 330, 330, 335, 320, \
   310, 320, 320, 320, 320, 320, 320, 310, \

#define ROOK_MG_PST \
   500, 500, 500, 500, 500, 500, 500, 500, \
   505, 530, 530, 530, 530, 530, 530, 505, \
   495, 510, 510, 510, 510, 510, 510, 495, \
   495, 500, 500, 500, 500, 500, 500, 495, \
   495, 500, 500, 500, 500, 500, 500, 495, \
   505, 510, 510, 500, 500, 510, 510, 505, \
   495, 495, 495, 500, 500, 495, 495, 495, \
   500, 500, 505, 515, 515, 505, 500, 500, \

#define ROOK_EG_PST \
   500, 500, 500, 500, 500, 500, 500, 500, \
   505, 530, 530, 530, 530, 530, 530, 505, \
   495, 510, 510, 510, 510, 510, 510, 495, \
   495, 500, 500, 500, 500, 500, 500, 495, \
   495, 500, 500, 500, 500, 500, 500, 495, \
   495, 500, 500, 500, 500, 500, 500, 495, \
   495, 510, 510, 500, 500, 510, 510, 495, \
   500, 500, 500, 510, 510, 500, 500, 500, \

#define QUEEN_MG_PST \
   880, 890, 890, 895, 895, 890, 890, 880, \
   890, 900, 900, 900, 900, 900, 900, 890, \
   890, 900, 905, 905, 905, 905, 900, 890, \
   895, 900, 905, 905, 905, 905, 900, 895, \
   895, 900, 905, 905, 905, 905, 900, 895, \
   890, 905, 905, 905, 905, 905, 900, 890, \
   890, 900, 905, 900, 900, 900, 900, 890, \
   880, 890, 890, 895, 895, 890, 890, 880, \

#define QUEEN_EG_PST \
   880, 890, 890, 895, 895, 890, 890, 880, \
   890, 900, 900, 900, 900, 900, 900, 890, \
   890, 900, 905, 905, 905, 905, 900, 890, \
   895, 900, 905, 905, 905, 905, 900, 895, \
   895, 900, 905, 905, 905, 905, 900, 895, \
   890, 905, 905, 905, 905, 905, 900, 890, \
   890, 900, 905, 900, 900, 900, 900, 890, \
   880, 890, 890, 895, 895, 890, 890, 880, \

#define KING_MG_PST \
   -30, -40, -40, -50, -50, -40, -40, -30, \
   -30, -40, -40, -50, -50, -40, -40, -30, \
   -30, -40, -40, -50, -50, -40, -40, -30, \
   -30, -40, -40, -50, -50, -40, -40, -30, \
   -20, -30, -30, -40, -40, -30, -30, -20, \
   -10, -20, -20, -20, -20, -20, -20, -10, \
   20, 20, 0, 0, 0, 0, 20, 20, \
   30, 40, 20, 0, 0, 10, 40, 30, \

#define KING_EG_PST \
   -50, -40, -30, -20, -20, -30, -40, -50, \
   -30, -20, -10, 0, 0, -10, -20, -30, \
   -30, -10, 20, 30, 30, 20, -10, -30, \
   -30, -10, 30, 40, 40, 30, -10, -30, \
   -30, -10, 30, 40, 40, 30, -10, -30, \
   -30, -10, 20, 30, 30, 20, -10, -30, \
   -30, -30, 0, 0, 0, 0, -30, -30, \
   -50, -30, -30, -30, -30, -30, -30, -50, \

#endif