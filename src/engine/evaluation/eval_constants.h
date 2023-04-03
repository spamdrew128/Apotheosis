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
Knight Values: 472 355
Bishop Values: 484 388
Rook Values: 661 713
Queen Values: 1319 1326
Pawn Values: 108 230
*/

#define KNIGHT_MG_PST \
   248, 293, 398, 451, 519, 368, 414, 342, \
   398, 438, 519, 547, 516, 585, 473, 437, \
   431, 516, 536, 569, 617, 658, 578, 500, \
   462, 495, 516, 556, 525, 582, 492, 519, \
   453, 474, 491, 494, 507, 499, 502, 463, \
   445, 464, 489, 483, 504, 495, 498, 447, \
   424, 433, 458, 470, 472, 488, 466, 448, \
   323, 444, 398, 424, 441, 440, 443, 398, \

#define KNIGHT_EG_PST \
   295, 350, 344, 332, 326, 346, 295, 243, \
   331, 368, 356, 357, 365, 327, 339, 326, \
   347, 355, 384, 377, 355, 337, 352, 326, \
   356, 381, 399, 398, 404, 384, 391, 350, \
   353, 382, 404, 405, 407, 401, 374, 349, \
   342, 370, 376, 397, 393, 373, 365, 348, \
   315, 344, 367, 371, 371, 356, 345, 329, \
   337, 300, 352, 358, 353, 348, 312, 307, \

#define BISHOP_MG_PST \
   455, 434, 410, 403, 399, 423, 416, 446, \
   441, 501, 493, 473, 521, 508, 502, 430, \
   450, 513, 514, 533, 548, 586, 526, 495, \
   469, 480, 512, 554, 535, 529, 490, 467, \
   470, 492, 494, 517, 520, 499, 496, 477, \
   488, 500, 496, 501, 496, 506, 497, 485, \
   498, 502, 499, 478, 488, 504, 521, 486, \
   438, 467, 464, 439, 456, 460, 460, 446, \

#define BISHOP_EG_PST \
   373, 378, 379, 383, 388, 381, 377, 364, \
   382, 389, 386, 382, 382, 377, 389, 371, \
   398, 393, 404, 391, 387, 390, 395, 384, \
   394, 409, 410, 405, 419, 408, 404, 390, \
   380, 401, 415, 419, 411, 409, 389, 373, \
   376, 392, 410, 410, 414, 401, 389, 377, \
   361, 364, 378, 393, 394, 388, 370, 356, \
   370, 385, 353, 391, 382, 371, 375, 367, \

#define ROOK_MG_PST \
   704, 708, 676, 733, 732, 703, 715, 765, \
   680, 688, 722, 766, 761, 753, 681, 712, \
   637, 683, 686, 724, 726, 729, 733, 709, \
   612, 638, 650, 670, 666, 690, 676, 651, \
   600, 602, 622, 636, 643, 634, 648, 633, \
   587, 604, 616, 620, 637, 630, 656, 614, \
   579, 610, 616, 620, 629, 641, 666, 558, \
   615, 617, 635, 641, 648, 630, 610, 629, \

#define ROOK_EG_PST \
   720, 721, 734, 716, 719, 719, 715, 701, \
   724, 729, 720, 708, 700, 703, 719, 707, \
   724, 719, 724, 708, 701, 702, 695, 693, \
   727, 724, 733, 722, 717, 712, 707, 708, \
   715, 727, 729, 722, 710, 708, 701, 694, \
   711, 718, 718, 713, 705, 701, 692, 689, \
   715, 708, 713, 716, 704, 702, 680, 708, \
   714, 725, 728, 730, 717, 713, 727, 682, \

#define QUEEN_MG_PST \
   1264, 1291, 1309, 1318, 1368, 1407, 1442, 1342, \
   1299, 1275, 1292, 1289, 1291, 1385, 1317, 1351, \
   1288, 1309, 1308, 1339, 1375, 1429, 1401, 1386, \
   1274, 1289, 1296, 1311, 1306, 1331, 1321, 1317, \
   1304, 1286, 1305, 1302, 1312, 1307, 1321, 1319, \
   1292, 1322, 1297, 1309, 1319, 1320, 1332, 1316, \
   1280, 1312, 1333, 1324, 1328, 1342, 1333, 1314, \
   1296, 1282, 1305, 1330, 1307, 1263, 1269, 1309, \

#define QUEEN_EG_PST \
   1320, 1344, 1351, 1361, 1348, 1309, 1272, 1329, \
   1298, 1338, 1378, 1395, 1440, 1351, 1352, 1328, \
   1302, 1315, 1356, 1368, 1366, 1339, 1318, 1330, \
   1324, 1345, 1355, 1371, 1416, 1400, 1395, 1365, \
   1296, 1352, 1349, 1387, 1370, 1370, 1356, 1343, \
   1289, 1278, 1341, 1325, 1327, 1327, 1320, 1302, \
   1278, 1289, 1259, 1291, 1286, 1258, 1248, 1266, \
   1282, 1293, 1272, 1237, 1266, 1294, 1290, 1230, \

#define PAWN_MG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   123, 192, 83, 131, 147, 42, 66, -35, \
   74, 121, 144, 167, 200, 182, 156, 78, \
   68, 118, 113, 132, 145, 128, 134, 71, \
   53, 99, 101, 127, 130, 119, 123, 70, \
   58, 97, 100, 94, 112, 112, 155, 86, \
   46, 103, 78, 74, 85, 138, 161, 75, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define PAWN_EG_PST \
   0, 0, 0, 0, 0, 0, 0, 0, \
   578, 542, 552, 493, 512, 526, 570, 610, \
   288, 293, 262, 236, 210, 213, 263, 268, \
   194, 177, 159, 141, 129, 140, 160, 164, \
   165, 153, 131, 122, 121, 124, 140, 137, \
   150, 150, 129, 139, 135, 128, 129, 126, \
   165, 155, 150, 147, 148, 135, 136, 130, \
   0, 0, 0, 0, 0, 0, 0, 0, \

#define KING_MG_PST \
   172, 71, 130, 144, 7, 22, 144, 40, \
   165, 79, 110, -6, 25, 40, 43, 34, \
   71, 81, 84, 42, 40, 140, 81, 31, \
   25, -13, -28, -44, -36, -28, -54, -115, \
   -69, 10, -77, -130, -113, -96, -82, -136, \
   -12, -1, -66, -121, -98, -81, -19, -45, \
   56, 8, -42, -109, -95, -44, 32, 37, \
   31, 75, 33, -107, -4, -59, 58, 52, \

#define KING_EG_PST \
   -144, -49, -60, -59, -32, -2, -43, -43, \
   -54, 11, -2, 14, 20, 37, 35, 3, \
   -12, 15, 15, 26, 28, 34, 45, 11, \
   -20, 30, 37, 49, 42, 51, 50, 16, \
   -17, -4, 41, 56, 60, 49, 29, 5, \
   -29, -4, 29, 52, 51, 39, 9, -8, \
   -52, -12, 14, 33, 38, 19, -10, -32, \
   -88, -66, -40, -3, -37, -11, -51, -86, \

/*

██████╗  ██████╗ ███╗   ██╗██╗   ██╗███████╗███████╗███████╗
██╔══██╗██╔═══██╗████╗  ██║██║   ██║██╔════╝██╔════╝██╔════╝
██████╔╝██║   ██║██╔██╗ ██║██║   ██║███████╗█████╗  ███████╗
██╔══██╗██║   ██║██║╚██╗██║██║   ██║╚════██║██╔══╝  ╚════██║
██████╔╝╚██████╔╝██║ ╚████║╚██████╔╝███████║███████╗███████║
╚═════╝  ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚══════╝╚══════╝╚══════╝

*/

#define BISHOP_PAIR_BONUS \
   36, 66

#endif