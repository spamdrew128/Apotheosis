#ifndef __EVAL_CONSTANTS_H__
#define __EVAL_CONSTANTS_H__

#include <stdint.h>

#include "board_constants.h"
#include "board_info.h"

typedef uint8_t Phase_t;
typedef uint8_t Bucket_t;
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

  QS_PST_MASK = C64(0x0F0F0F0F0F0F0F0F),
  KS_PST_MASK = C64(0xF0F0F0F0F0F0F0F0),
  NUM_PST_BUCKETS = 2,
};

// used for eval calcuations. Contains both EG and MG score packed into 1 value.
typedef int32_t Score_t;
#define S(mg, eg) \
	((Score_t)((uint32_t)eg << 16) + mg)

/*

 ██████╗ ██████╗ ███╗   ██╗███████╗████████╗ █████╗ ███╗   ██╗████████╗███████╗
██╔════╝██╔═══██╗████╗  ██║██╔════╝╚══██╔══╝██╔══██╗████╗  ██║╚══██╔══╝██╔════╝
██║     ██║   ██║██╔██╗ ██║███████╗   ██║   ███████║██╔██╗ ██║   ██║   ███████╗
██║     ██║   ██║██║╚██╗██║╚════██║   ██║   ██╔══██║██║╚██╗██║   ██║   ╚════██║
╚██████╗╚██████╔╝██║ ╚████║███████║   ██║   ██║  ██║██║ ╚████║   ██║   ███████║
 ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝

*/                                                                        

#define BISHOP_PAIR_BONUS \
   S(43, 59)

#define PASSED_PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(4, 333), S(7, 275), S(-19, 290), S(6, 301), S(45, 343), S(-3, 356), S(-20, 364), S(38, 363), \
   S(-28, 182), S(-27, 173), S(5, 117), S(-106, 118), S(-11, 119), S(-34, 219), S(-70, 256), S(-81, 300), \
   S(-16, 85), S(14, 70), S(31, 52), S(29, 42), S(-28, 78), S(-60, 108), S(-68, 147), S(-115, 184), \
   S(3, 47), S(-3, 43), S(3, 24), S(1, 15), S(-55, 40), S(-67, 54), S(-22, 76), S(-58, 92), \
   S(39, 11), S(7, 22), S(39, 5), S(-4, -1), S(-41, 15), S(-29, 21), S(-52, 41), S(-11, 32), \
   S(8, 3), S(83, 14), S(85, 5), S(-19, 2), S(-34, 12), S(-28, 11), S(-28, 29), S(-24, 27), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(82, 361), S(75, 344), S(33, 351), S(69, 314), S(88, 296), S(30, 283), S(6, 310), S(-21, 344), \
   S(48, 246), S(9, 262), S(3, 206), S(29, 153), S(8, 97), S(57, 110), S(-7, 110), S(-14, 162), \
   S(13, 135), S(6, 128), S(12, 95), S(7, 80), S(15, 57), S(61, 43), S(18, 70), S(21, 71), \
   S(9, 72), S(-21, 75), S(-26, 58), S(-7, 35), S(-12, 34), S(-13, 37), S(17, 51), S(40, 39), \
   S(0, 28), S(-7, 34), S(-31, 30), S(-25, 20), S(2, 7), S(13, 11), S(35, 32), S(48, 10), \
   S(-1, 21), S(5, 27), S(0, 11), S(-26, 25), S(31, 5), S(31, 5), S(65, 27), S(24, 18), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define BLOCKED_PASSERS { \
   S(139, -420), S(27, -130), S(-1, -49), S(-5, -18), S(-1, 1), S(-21, 2), S(0, 0), S(0, 0), \
}

#define ROOK_OPEN_FILE { \
   S(59, -7), S(69, -10), S(60, -5), S(59, -1), S(59, 2), S(88, -13), S(136, -21), S(125, -19), \
}

#define ROOK_SEMI_OPEN_FILE { \
   S(2, 54), S(23, 24), S(26, 13), S(29, 6), S(32, -4), S(36, -7), S(60, -8), S(34, 36), \
}

#define QUEEN_OPEN_FILE { \
   S(4, 20), S(2, 29), S(-18, 36), S(-17, 46), S(-4, 47), S(33, 1), S(53, -16), S(87, -23), \
}

#define QUEEN_SEMI_OPEN_FILE { \
   S(1, 18), S(16, 12), S(5, 17), S(-5, 51), S(10, 27), S(30, -30), S(52, -64), S(57, -24), \
}

#define KING_OPEN_FILE { \
   S(-87, -12), S(-119, 1), S(-90, -2), S(-42, -10), S(-63, -4), S(-91, 7), S(-107, 14), S(-61, 2), \
}

#define KING_SEMI_OPEN_FILE { \
   S(-39, 41), S(-55, 19), S(-21, -2), S(-11, -6), S(-17, -2), S(-30, 2), S(-42, 18), S(-45, 35), \
}

#define ISOLATED_PAWNS { \
   S(-14, -20), S(-22, -15), S(-23, -17), S(-37, -12), S(-33, -19), S(-11, -17), S(-17, -17), S(-30, -15), \
}

#define BOARD_CONTROL_BONUSES { { \
   S(70, -8), S(56, -4), S(13, 6), S(44, -3), S(34, 0), S(17, -1), S(-1, 7), S(6, 9), \
   S(45, -4), S(32, -3), S(27, -3), S(29, 0), S(33, -7), S(-3, 4), S(5, 3), S(-9, 9), \
   S(15, 5), S(42, -6), S(44, -6), S(36, -5), S(14, -2), S(-1, 5), S(1, 4), S(1, 2), \
   S(22, 4), S(14, 6), S(19, 0), S(12, 0), S(21, -1), S(13, 1), S(15, 1), S(11, 0), \
   S(14, 8), S(17, 1), S(7, 4), S(14, -1), S(23, -4), S(24, -7), S(13, -5), S(11, -3), \
   S(7, 13), S(7, 8), S(1, 5), S(9, 2), S(2, 2), S(0, 3), S(8, 0), S(15, 0), \
   S(8, 4), S(5, 9), S(11, 3), S(4, 2), S(4, 2), S(4, 2), S(4, 1), S(4, 2), \
   S(7, 5), S(-2, 7), S(-4, 11), S(5, 5), S(13, 5), S(6, 4), S(9, 0), S(-16, 10), \
}, { \
   S(24, -1), S(13, 4), S(23, 0), S(12, 2), S(18, 1), S(25, -5), S(23, -8), S(59, -9), \
   S(0, 7), S(5, 5), S(4, 2), S(17, -2), S(28, -8), S(17, -2), S(35, -4), S(26, -2), \
   S(3, 6), S(6, 2), S(3, 4), S(6, 5), S(16, 1), S(30, -2), S(21, 0), S(8, 7), \
   S(0, 7), S(4, 3), S(4, 2), S(7, 2), S(4, 4), S(6, 5), S(7, 5), S(7, 7), \
   S(5, 4), S(4, 1), S(6, 1), S(6, 1), S(7, 0), S(6, 4), S(6, 0), S(4, 6), \
   S(0, 7), S(6, -1), S(6, 0), S(8, 0), S(4, -2), S(2, 3), S(2, 7), S(10, 4), \
   S(0, 6), S(6, 3), S(0, 1), S(0, 5), S(-1, 3), S(-6, 7), S(3, 3), S(-7, 10), \
   S(3, 5), S(-3, 11), S(5, 6), S(7, 6), S(8, 6), S(2, 8), S(0, 2), S(-6, 6), \
}, }

/*
Average PST values for MG, EG:
Knight Values: 419 349
Bishop Values: 413 379
Rook Values: 518 701
Queen Values: 1158 1294
Pawn Values: 96 154
*/

#define KNIGHT_PST { { \
   S(150, 232), S(328, 246), S(183, 317), S(397, 296), S(471, 283), S(415, 281), S(373, 290), S(347, 225), \
   S(365, 268), S(426, 289), S(331, 319), S(436, 316), S(437, 321), S(481, 296), S(375, 316), S(299, 320), \
   S(382, 292), S(429, 294), S(351, 333), S(446, 316), S(419, 338), S(455, 311), S(428, 335), S(347, 309), \
   S(386, 326), S(400, 328), S(442, 341), S(445, 352), S(471, 348), S(461, 345), S(461, 324), S(377, 329), \
   S(409, 302), S(444, 317), S(415, 345), S(442, 342), S(424, 357), S(428, 345), S(402, 330), S(359, 338), \
   S(399, 286), S(411, 324), S(411, 323), S(425, 335), S(454, 331), S(419, 327), S(412, 328), S(347, 335), \
   S(335, 286), S(399, 311), S(397, 319), S(408, 338), S(399, 338), S(396, 332), S(369, 323), S(346, 316), \
   S(350, 251), S(376, 282), S(323, 334), S(387, 308), S(340, 321), S(382, 303), S(385, 257), S(294, 298), \
}, { \
   S(237, 358), S(255, 411), S(361, 389), S(382, 386), S(452, 375), S(306, 395), S(361, 330), S(311, 268), \
   S(358, 377), S(395, 407), S(460, 389), S(492, 394), S(461, 407), S(476, 363), S(432, 360), S(414, 358), \
   S(399, 388), S(449, 392), S(481, 410), S(503, 405), S(549, 392), S(605, 374), S(490, 393), S(469, 368), \
   S(448, 390), S(482, 405), S(482, 421), S(525, 432), S(489, 436), S(522, 426), S(461, 428), S(492, 387), \
   S(439, 391), S(462, 409), S(474, 417), S(480, 431), S(494, 423), S(476, 419), S(494, 398), S(452, 380), \
   S(441, 374), S(458, 389), S(479, 385), S(478, 405), S(495, 411), S(485, 386), S(495, 385), S(446, 384), \
   S(428, 340), S(436, 359), S(457, 387), S(473, 385), S(475, 391), S(483, 376), S(482, 370), S(455, 360), \
   S(337, 362), S(445, 333), S(415, 373), S(427, 386), S(444, 389), S(452, 377), S(442, 358), S(419, 320), \
}, }

#define BISHOP_PST { { \
   S(250, 367), S(314, 363), S(317, 339), S(275, 348), S(216, 363), S(247, 363), S(362, 333), S(369, 337), \
   S(368, 322), S(315, 348), S(268, 366), S(332, 341), S(401, 349), S(349, 354), S(376, 358), S(271, 363), \
   S(437, 325), S(484, 319), S(329, 356), S(377, 362), S(433, 345), S(423, 341), S(368, 359), S(364, 357), \
   S(391, 343), S(383, 341), S(380, 371), S(435, 341), S(398, 370), S(420, 353), S(428, 354), S(373, 352), \
   S(378, 332), S(354, 351), S(391, 352), S(404, 353), S(435, 341), S(432, 350), S(388, 347), S(357, 352), \
   S(371, 334), S(404, 338), S(400, 343), S(403, 345), S(419, 357), S(417, 352), S(429, 356), S(403, 355), \
   S(442, 313), S(416, 331), S(408, 320), S(411, 332), S(412, 337), S(429, 348), S(454, 325), S(363, 343), \
   S(287, 339), S(341, 354), S(392, 323), S(351, 356), S(380, 341), S(378, 330), S(363, 352), S(323, 356), \
}, { \
   S(442, 402), S(412, 402), S(364, 419), S(352, 424), S(380, 419), S(360, 424), S(400, 419), S(418, 395), \
   S(406, 428), S(434, 421), S(444, 405), S(397, 415), S(443, 415), S(449, 411), S(401, 423), S(387, 403), \
   S(416, 424), S(467, 425), S(447, 428), S(467, 421), S(479, 427), S(479, 437), S(480, 416), S(453, 415), \
   S(441, 421), S(464, 421), S(460, 441), S(504, 433), S(477, 430), S(463, 436), S(459, 419), S(439, 420), \
   S(453, 413), S(459, 422), S(472, 439), S(479, 439), S(478, 423), S(470, 417), S(463, 404), S(460, 397), \
   S(461, 409), S(483, 419), S(472, 421), S(480, 421), S(477, 424), S(484, 413), S(483, 396), S(460, 401), \
   S(480, 393), S(489, 378), S(484, 391), S(465, 406), S(473, 409), S(493, 407), S(512, 384), S(474, 374), \
   S(433, 387), S(467, 398), S(450, 388), S(440, 414), S(456, 402), S(448, 402), S(466, 394), S(445, 378), \
}, }

#define ROOK_PST { { \
   S(417, 630), S(364, 655), S(365, 659), S(362, 667), S(386, 668), S(414, 656), S(381, 659), S(443, 637), \
   S(478, 623), S(487, 633), S(528, 618), S(486, 643), S(464, 645), S(482, 658), S(424, 660), S(426, 658), \
   S(445, 639), S(503, 625), S(546, 629), S(507, 629), S(458, 645), S(435, 676), S(428, 659), S(418, 639), \
   S(452, 626), S(418, 644), S(432, 642), S(468, 649), S(427, 665), S(485, 661), S(382, 664), S(448, 635), \
   S(386, 638), S(407, 643), S(425, 647), S(417, 654), S(427, 659), S(444, 662), S(378, 661), S(396, 652), \
   S(399, 621), S(429, 623), S(464, 617), S(477, 613), S(430, 634), S(458, 640), S(395, 651), S(445, 615), \
   S(437, 591), S(414, 631), S(458, 610), S(443, 624), S(420, 635), S(434, 650), S(382, 633), S(405, 622), \
   S(480, 588), S(482, 604), S(477, 615), S(479, 627), S(484, 626), S(476, 632), S(410, 647), S(446, 617), \
}, { \
   S(596, 763), S(559, 793), S(521, 809), S(565, 788), S(546, 806), S(527, 805), S(561, 791), S(626, 762), \
   S(599, 780), S(594, 798), S(622, 792), S(668, 770), S(664, 766), S(632, 780), S(538, 812), S(578, 772), \
   S(583, 763), S(609, 769), S(606, 778), S(633, 764), S(634, 760), S(676, 748), S(624, 765), S(611, 747), \
   S(572, 767), S(590, 768), S(603, 785), S(608, 766), S(594, 774), S(633, 764), S(597, 771), S(586, 753), \
   S(583, 753), S(554, 779), S(579, 785), S(587, 770), S(595, 762), S(591, 764), S(578, 767), S(603, 728), \
   S(576, 748), S(573, 762), S(578, 773), S(585, 763), S(603, 753), S(606, 752), S(604, 752), S(598, 723), \
   S(576, 739), S(589, 744), S(582, 759), S(592, 754), S(605, 742), S(625, 739), S(633, 729), S(546, 729), \
   S(614, 733), S(598, 746), S(608, 756), S(617, 747), S(621, 745), S(629, 740), S(579, 745), S(625, 690), \
}, }

#define QUEEN_PST { { \
   S(1012, 1135), S(1015, 1156), S(1131, 1104), S(930, 1186), S(1068, 1145), S(1033, 1165), S(1154, 1075), S(965, 1157), \
   S(1107, 1099), S(1100, 1092), S(1148, 1111), S(1029, 1210), S(1109, 1166), S(1052, 1180), S(973, 1226), S(1031, 1147), \
   S(1116, 1077), S(1122, 1129), S(1213, 1118), S(1129, 1123), S(1069, 1183), S(1025, 1205), S(1008, 1216), S(985, 1193), \
   S(1040, 1160), S(1062, 1128), S(1021, 1169), S(1041, 1165), S(1064, 1124), S(1029, 1194), S(989, 1219), S(1041, 1161), \
   S(1061, 1132), S(1024, 1170), S(1021, 1253), S(1043, 1159), S(1033, 1126), S(1016, 1234), S(997, 1212), S(986, 1169), \
   S(1072, 1099), S(1048, 1139), S(1047, 1133), S(1048, 1115), S(1050, 1143), S(1077, 1127), S(1014, 1206), S(1011, 1141), \
   S(1057, 1085), S(1040, 1116), S(1104, 1075), S(1046, 1131), S(1079, 1092), S(1082, 1124), S(986, 1187), S(1006, 1111), \
   S(998, 1137), S(1040, 1109), S(1049, 1121), S(1082, 1059), S(1077, 1078), S(1029, 1155), S(983, 1174), S(973, 1126), \
}, { \
   S(1228, 1406), S(1215, 1455), S(1224, 1459), S(1232, 1465), S(1266, 1459), S(1299, 1464), S(1310, 1467), S(1262, 1492), \
   S(1244, 1425), S(1228, 1438), S(1251, 1474), S(1244, 1484), S(1219, 1552), S(1319, 1501), S(1227, 1543), S(1259, 1511), \
   S(1250, 1411), S(1269, 1417), S(1262, 1440), S(1291, 1464), S(1324, 1461), S(1330, 1498), S(1327, 1480), S(1295, 1494), \
   S(1235, 1444), S(1247, 1446), S(1261, 1461), S(1267, 1459), S(1244, 1502), S(1261, 1542), S(1248, 1564), S(1261, 1497), \
   S(1279, 1406), S(1245, 1454), S(1268, 1456), S(1263, 1457), S(1262, 1465), S(1267, 1491), S(1262, 1501), S(1275, 1470), \
   S(1266, 1394), S(1288, 1382), S(1268, 1430), S(1281, 1415), S(1292, 1408), S(1284, 1441), S(1291, 1455), S(1276, 1429), \
   S(1265, 1382), S(1288, 1382), S(1304, 1362), S(1303, 1381), S(1305, 1372), S(1305, 1382), S(1300, 1382), S(1272, 1415), \
   S(1272, 1369), S(1256, 1386), S(1282, 1377), S(1309, 1347), S(1294, 1361), S(1247, 1399), S(1249, 1410), S(1258, 1342), \
}, }

#define PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(4, 333), S(7, 275), S(-19, 290), S(6, 301), S(45, 343), S(-3, 356), S(-20, 364), S(38, 363), \
   S(100, 141), S(158, 101), S(163, 128), S(243, 136), S(129, 176), S(140, 129), S(113, 159), S(123, 148), \
   S(42, 136), S(77, 107), S(99, 113), S(134, 107), S(138, 116), S(131, 117), S(125, 125), S(126, 125), \
   S(42, 122), S(45, 110), S(99, 103), S(121, 107), S(120, 112), S(122, 108), S(94, 115), S(109, 115), \
   S(35, 117), S(54, 101), S(85, 111), S(87, 116), S(108, 117), S(114, 112), S(115, 106), S(116, 106), \
   S(36, 124), S(67, 99), S(100, 106), S(102, 103), S(107, 113), S(153, 104), S(128, 99), S(102, 108), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(82, 361), S(75, 344), S(33, 351), S(69, 314), S(88, 296), S(30, 283), S(6, 310), S(-21, 344), \
   S(77, 171), S(125, 140), S(140, 137), S(142, 141), S(195, 151), S(178, 131), S(161, 155), S(110, 152), \
   S(80, 150), S(116, 123), S(113, 121), S(138, 100), S(139, 106), S(127, 104), S(96, 115), S(97, 118), \
   S(64, 137), S(98, 114), S(105, 111), S(132, 107), S(125, 109), S(129, 98), S(91, 101), S(93, 107), \
   S(72, 122), S(94, 105), S(104, 110), S(102, 117), S(112, 124), S(118, 108), S(128, 90), S(107, 101), \
   S(60, 134), S(102, 104), S(84, 129), S(89, 127), S(86, 138), S(147, 112), S(132, 97), S(100, 98), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define KING_PST { { \
   S(55, -97), S(125, -34), S(90, -74), S(223, -82), S(352, -26), S(428, -6), S(421, -35), S(333, -49), \
   S(61, -39), S(213, 21), S(137, 12), S(23, 16), S(371, 41), S(443, 32), S(454, 18), S(505, -44), \
   S(110, -16), S(125, 11), S(264, 24), S(-7, 37), S(397, 46), S(485, 37), S(478, 17), S(426, 2), \
   S(-56, -21), S(42, 18), S(25, 31), S(-93, 61), S(334, 66), S(313, 66), S(402, 48), S(384, 14), \
   S(-120, -7), S(-64, 13), S(-105, 47), S(-172, 68), S(247, 82), S(351, 52), S(382, 22), S(298, 7), \
   S(-107, -22), S(-37, 1), S(-129, 41), S(-156, 54), S(313, 59), S(310, 44), S(382, 12), S(396, -17), \
   S(16, -43), S(0, -13), S(-40, 7), S(-140, 31), S(331, 40), S(361, 18), S(412, -10), S(411, -32), \
   S(12, -93), S(52, -71), S(-38, -32), S(-134, -1), S(368, -17), S(325, -7), S(427, -52), S(423, -83), \
}, { \
   S(-162, -157), S(-249, -66), S(-232, -57), S(-309, -35), S(-20, -21), S(66, -23), S(316, -84), S(73, -56), \
   S(-182, -63), S(-263, -2), S(-256, -1), S(-372, 21), S(88, 13), S(98, 16), S(123, 21), S(-48, 20), \
   S(-300, -12), S(-252, 7), S(-309, 17), S(-352, 38), S(70, 38), S(273, 28), S(194, 44), S(41, 7), \
   S(-343, -8), S(-370, 29), S(-385, 36), S(-437, 60), S(-32, 56), S(38, 44), S(-71, 48), S(-190, 23), \
   S(-447, -8), S(-332, -5), S(-433, 39), S(-517, 61), S(-111, 74), S(-110, 58), S(-87, 30), S(-152, 6), \
   S(-389, -29), S(-385, -3), S(-448, 27), S(-519, 53), S(-107, 66), S(-77, 46), S(-24, 14), S(-45, -15), \
   S(-300, -75), S(-355, -25), S(-428, 10), S(-480, 27), S(-106, 51), S(-48, 28), S(21, -3), S(25, -33), \
   S(-322, -112), S(-284, -94), S(-326, -58), S(-471, -8), S(-16, -20), S(-69, -2), S(38, -45), S(37, -91), \
}, }

#endif