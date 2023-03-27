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

/*
Average PST values for MG, EG:
Knight Values: 320 287
Bishop Values: 361 328
Rook Values: 485 550
Queen Values: 963 1079
Pawn Values: 89 158
*/

#define PAWN_MG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   171, 109, 210, 208, 221, 181, 128, 154, \
   57, 92, 97, 134, 118, 127, 112, 80, \
   50, 72, 80, 99, 91, 85, 83, 43, \
   39, 65, 65, 83, 84, 70, 79, 37, \
   40, 69, 68, 57, 61, 69, 98, 42, \
   24, 66, 57, 43, 42, 77, 93, 29, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define PAWN_EG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   276, 308, 267, 260, 227, 261, 298, 259, \
   201, 193, 178, 175, 178, 151, 182, 184, \
   151, 141, 132, 112, 119, 116, 128, 141, \
   131, 129, 119, 118, 125, 115, 129, 127, \
   131, 124, 115, 120, 129, 121, 114, 116, \
   139, 132, 129, 124, 109, 126, 119, 124, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define KNIGHT_MG_PST \
   140, 331, 132, 339, 304, 239, 215, 162, \
   293, 294, 365, 429, 371, 415, 289, 317, \
   312, 387, 360, 399, 397, 407, 371, 340, \
   329, 332, 370, 363, 358, 390, 330, 362, \
   314, 314, 341, 336, 343, 346, 338, 338, \
   301, 324, 328, 342, 329, 330, 337, 306, \
   285, 297, 316, 316, 322, 325, 311, 308, \
   248, 301, 313, 280, 294, 291, 303, 275, \

#define KNIGHT_EG_PST \
   212, 289, 339, 278, 297, 319, 275, 197, \
   252, 301, 306, 295, 291, 268, 289, 253, \
   292, 295, 312, 307, 306, 295, 304, 267, \
   303, 316, 321, 330, 343, 317, 323, 282, \
   292, 293, 319, 327, 318, 322, 299, 284, \
   259, 292, 305, 311, 321, 309, 298, 246, \
   246, 263, 284, 288, 300, 279, 260, 250, \
   239, 235, 269, 281, 270, 284, 233, 214, \

#define BISHOP_MG_PST \
   312, 341, 343, 219, 334, 360, 356, 347, \
   332, 375, 348, 369, 371, 365, 370, 328, \
   365, 391, 410, 440, 388, 448, 408, 383, \
   358, 363, 384, 403, 396, 388, 365, 365, \
   357, 381, 374, 382, 381, 368, 357, 356, \
   354, 367, 365, 363, 362, 371, 373, 358, \
   350, 355, 363, 356, 360, 379, 369, 339, \
   298, 310, 338, 313, 369, 340, 330, 323, \

#define BISHOP_EG_PST \
   350, 345, 326, 334, 325, 314, 311, 354, \
   311, 334, 335, 341, 334, 337, 335, 317, \
   331, 317, 332, 329, 338, 332, 345, 333, \
   318, 336, 330, 331, 351, 339, 334, 326, \
   330, 325, 344, 334, 342, 357, 351, 317, \
   316, 331, 333, 346, 342, 334, 336, 335, \
   328, 307, 326, 320, 331, 319, 321, 305, \
   289, 337, 271, 338, 304, 288, 325, 302, \

#define ROOK_MG_PST \
   570, 546, 567, 620, 606, 628, 575, 601, \
   517, 523, 575, 582, 590, 576, 534, 537, \
   493, 554, 508, 540, 535, 545, 547, 514, \
   445, 463, 475, 499, 485, 475, 476, 476, \
   432, 430, 446, 460, 456, 444, 452, 425, \
   423, 437, 436, 428, 438, 432, 461, 424, \
   388, 443, 439, 426, 416, 434, 443, 371, \
   426, 442, 443, 451, 451, 450, 424, 411, \

#define ROOK_EG_PST \
   540, 548, 540, 529, 533, 528, 543, 537, \
   562, 557, 549, 555, 552, 555, 553, 556, \
   554, 538, 554, 553, 545, 538, 531, 548, \
   566, 566, 571, 559, 560, 553, 552, 554, \
   553, 571, 567, 563, 555, 557, 568, 550, \
   551, 552, 553, 557, 545, 555, 538, 548, \
   545, 542, 548, 546, 553, 542, 531, 564, \
   539, 549, 549, 547, 547, 548, 561, 534, \

#define QUEEN_MG_PST \
   921, 1006, 1087, 1066, 1051, 1106, 1005, 996, \
   938, 937, 986, 967, 993, 974, 979, 994, \
   942, 987, 963, 977, 991, 1030, 1003, 992, \
   947, 935, 937, 954, 957, 949, 950, 959, \
   945, 942, 942, 946, 955, 945, 943, 950, \
   928, 946, 945, 948, 935, 942, 956, 943, \
   925, 946, 958, 949, 948, 963, 935, 959, \
   911, 932, 941, 948, 938, 919, 930, 964, \

#define QUEEN_EG_PST \
   1096, 1081, 1011, 1054, 1090, 1015, 1117, 1062, \
   1097, 1117, 1098, 1174, 1160, 1163, 1094, 1068, \
   1068, 1065, 1146, 1114, 1161, 1101, 1152, 1117, \
   1037, 1126, 1164, 1154, 1150, 1155, 1183, 1125, \
   1020, 1087, 1110, 1133, 1121, 1115, 1147, 1100, \
   1079, 1062, 1098, 1041, 1083, 1099, 1051, 999, \
   1082, 1019, 969, 1015, 1032, 1045, 979, 921, \
   1034, 1025, 969, 973, 1014, 1032, 1042, 1057, \

#define KING_MG_PST \
   395, -70, -103, -145, 192, 222, 0, 359, \
   -160, -30, 17, -27, -27, -45, 120, -89, \
   -2, -69, -50, -26, -126, -53, -132, -61, \
   -27, -67, -79, -63, -119, -24, -81, -73, \
   -108, -84, -46, -86, -53, -55, -73, -106, \
   44, -19, -66, -69, -67, -55, -50, -68, \
   35, 6, -19, -41, -33, -8, 11, -1, \
   10, 59, 34, -17, 29, -8, 39, 41, \

#define KING_EG_PST \
   -141, 19, 28, 56, 43, 5, 57, -115, \
   59, 51, 37, 32, 48, 71, 65, 53, \
   42, 71, 48, 21, 48, 63, 91, 59, \
   -5, 37, 32, 9, 17, 34, 54, 25, \
   13, 23, 12, 16, 16, 22, 32, 22, \
   -22, -4, 10, 15, 17, 12, 9, 12, \
   -40, -19, -10, 1, -3, -6, -19, -24, \
   -22, -62, -39, -43, -63, -44, -43, -64, \

#endif