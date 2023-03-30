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
Knight Values: 222 322
Bishop Values: 451 420
Rook Values: 650 782
Queen Values: 1201 1431
Pawn Values: 231 400
*/

#define PAWN_MG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   231, 281, 195, 259, 215, 233, 127, 53, \
   91, 108, 144, 160, 190, 199, 159, 100, \
   71, 115, 111, 127, 144, 130, 138, 81, \
   55, 96, 93, 120, 121, 111, 124, 74, \
   56, 92, 94, 84, 105, 106, 147, 89, \
   47, 95, 76, 65, 81, 129, 155, 77, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define PAWN_EG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   400, 387, 375, 325, 347, 320, 389, 429, \
   287, 300, 266, 233, 215, 207, 261, 265, \
   196, 178, 160, 137, 127, 140, 160, 165, \
   167, 158, 137, 125, 123, 128, 141, 140, \
   157, 155, 131, 169, 149, 135, 137, 129, \
   167, 157, 150, 171, 173, 145, 141, 134, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define KNIGHT_MG_PST \
   222, 350, 352, 412, 552, 314, 388, 304, \
   363, 397, 530, 497, 486, 541, 454, 446, \
   383, 511, 505, 542, 569, 604, 530, 484, \
   431, 452, 469, 514, 483, 536, 461, 476, \
   411, 437, 455, 452, 468, 456, 476, 423, \
   397, 418, 447, 445, 464, 451, 460, 403, \
   386, 382, 415, 424, 426, 451, 426, 417, \
   273, 396, 370, 389, 396, 402, 400, 392, \

#define KNIGHT_EG_PST \
   322, 353, 406, 369, 356, 382, 336, 263, \
   379, 406, 377, 405, 396, 374, 383, 344, \
   387, 387, 424, 415, 397, 396, 384, 359, \
   396, 422, 445, 443, 444, 431, 422, 385, \
   404, 409, 447, 449, 447, 445, 415, 397, \
   388, 421, 397, 433, 432, 405, 390, 387, \
   356, 393, 400, 425, 413, 385, 373, 356, \
   376, 344, 383, 389, 393, 388, 372, 327, \

#define BISHOP_MG_PST \
   451, 453, 361, 351, 394, 384, 456, 425, \
   452, 497, 453, 448, 530, 522, 513, 424, \
   448, 516, 523, 534, 542, 565, 531, 501, \
   460, 474, 503, 539, 522, 533, 482, 459, \
   460, 481, 483, 505, 510, 485, 478, 459, \
   474, 492, 484, 489, 489, 491, 484, 478, \
   485, 483, 495, 466, 473, 496, 506, 475, \
   434, 471, 447, 441, 455, 445, 461, 443, \

#define BISHOP_EG_PST \
   420, 415, 440, 444, 432, 429, 416, 413, \
   423, 431, 439, 428, 426, 418, 427, 416, \
   438, 428, 437, 429, 425, 432, 433, 425, \
   434, 452, 455, 452, 458, 443, 438, 447, \
   427, 447, 452, 461, 450, 442, 441, 426, \
   425, 426, 452, 439, 446, 447, 429, 419, \
   394, 426, 409, 448, 455, 420, 423, 399, \
   404, 414, 426, 431, 426, 436, 413, 411, \

#define ROOK_MG_PST \
   650, 682, 634, 687, 690, 640, 670, 699, \
   627, 631, 669, 696, 718, 716, 638, 658, \
   583, 622, 625, 646, 642, 686, 688, 639, \
   554, 598, 592, 618, 615, 623, 608, 594, \
   532, 550, 569, 581, 600, 574, 603, 568, \
   529, 555, 561, 566, 571, 579, 604, 556, \
   524, 558, 565, 568, 575, 586, 595, 503, \
   557, 564, 578, 592, 592, 574, 554, 557, \

#define ROOK_EG_PST \
   782, 771, 796, 778, 777, 782, 768, 763, \
   783, 789, 783, 775, 753, 758, 777, 770, \
   783, 782, 784, 777, 768, 751, 755, 759, \
   783, 772, 791, 777, 772, 770, 762, 767, \
   781, 783, 787, 783, 765, 766, 757, 758, \
   771, 775, 770, 770, 772, 756, 748, 746, \
   770, 765, 770, 777, 763, 762, 751, 767, \
   770, 779, 782, 774, 767, 772, 775, 754, \

#define QUEEN_MG_PST \
   1201, 1238, 1225, 1277, 1389, 1345, 1336, 1302, \
   1240, 1209, 1256, 1240, 1217, 1338, 1295, 1330, \
   1252, 1248, 1265, 1262, 1328, 1368, 1359, 1331, \
   1223, 1229, 1233, 1238, 1251, 1276, 1258, 1258, \
   1235, 1226, 1235, 1229, 1242, 1244, 1264, 1251, \
   1225, 1247, 1241, 1250, 1252, 1259, 1270, 1260, \
   1216, 1238, 1258, 1265, 1263, 1280, 1257, 1271, \
   1245, 1216, 1234, 1254, 1237, 1207, 1219, 1232, \

#define QUEEN_EG_PST \
   1431, 1442, 1488, 1453, 1397, 1417, 1405, 1449, \
   1392, 1446, 1467, 1495, 1541, 1435, 1439, 1402, \
   1392, 1411, 1436, 1490, 1461, 1443, 1399, 1424, \
   1419, 1453, 1452, 1493, 1506, 1499, 1498, 1471, \
   1427, 1449, 1449, 1498, 1473, 1463, 1456, 1449, \
   1415, 1412, 1437, 1400, 1419, 1408, 1395, 1398, \
   1386, 1404, 1411, 1344, 1373, 1354, 1350, 1330, \
   1379, 1404, 1395, 1431, 1394, 1392, 1379, 1321, \

#define KING_MG_PST \
   -25, 118, 150, 34, -121, -125, 114, 53, \
   150, 16, -39, 103, 13, -34, -5, -29, \
   -24, 39, 81, 4, 10, 111, 130, 5, \
   -12, -47, -32, -84, -99, -74, -43, -109, \
   -159, -23, -83, -159, -165, -112, -98, -127, \
   15, -28, -78, -126, -96, -93, -29, -51, \
   64, 28, -24, -82, -75, -36, 29, 42, \
   48, 91, 50, -96, 7, -46, 60, 64, \

#define KING_EG_PST \
   -106, -64, -47, -20, 15, 48, 6, -25, \
   -43, 35, 35, 12, 35, 73, 53, 28, \
   16, 31, 29, 35, 45, 58, 54, 25, \
   0, 48, 51, 63, 66, 69, 57, 28, \
   12, 11, 54, 76, 81, 63, 42, 16, \
   -22, 10, 39, 58, 59, 48, 23, 4, \
   -50, -13, 18, 34, 37, 23, -6, -30, \
   -97, -73, -41, 0, -32, -8, -48, -88, \

#endif