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
   S(38, 65)

#define PASSED_PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(26, 323), S(39, 269), S(9, 280), S(25, 297), S(61, 334), S(10, 349), S(-27, 364), S(21, 360), \
   S(-30, 159), S(-63, 149), S(-15, 107), S(-114, 105), S(-9, 103), S(-38, 205), S(-92, 237), S(-95, 284), \
   S(-20, 61), S(-6, 44), S(31, 37), S(21, 32), S(-26, 65), S(-73, 96), S(-96, 129), S(-133, 169), \
   S(-1, 24), S(-22, 16), S(-7, 12), S(-12, 6), S(-61, 27), S(-68, 39), S(-44, 58), S(-79, 79), \
   S(42, -12), S(-3, -4), S(35, -8), S(-16, -12), S(-50, 2), S(-33, 6), S(-64, 21), S(-26, 17), \
   S(16, -20), S(75, -13), S(79, -5), S(-28, -5), S(-46, 0), S(-28, -3), S(-42, 11), S(-44, 17), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(82, 353), S(93, 336), S(38, 346), S(78, 309), S(103, 285), S(43, 274), S(39, 297), S(-25, 335), \
   S(39, 220), S(-7, 237), S(0, 192), S(21, 138), S(5, 86), S(49, 97), S(-26, 93), S(-29, 150), \
   S(0, 111), S(-12, 104), S(6, 82), S(0, 71), S(9, 47), S(56, 30), S(-5, 52), S(6, 55), \
   S(-2, 49), S(-44, 51), S(-34, 45), S(-20, 27), S(-24, 24), S(-16, 23), S(-8, 32), S(26, 23), \
   S(-14, 7), S(-27, 12), S(-42, 19), S(-38, 9), S(0, -6), S(6, -1), S(20, 11), S(33, -5), \
   S(-14, 1), S(-8, 2), S(-4, -1), S(-37, 13), S(25, -7), S(30, -10), S(58, 5), S(8, 4), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define BLOCKED_PASSERS { \
S(153, -414), S(30, -128), S(2, -50), S(-8, -20), S(-2, -2), S(-19, -4), S(0, 0), S(0, 0), }

#define ROOK_OPEN_FILE { \
S(62, 10), S(73, 2), S(65, 2), S(68, 7), S(72, 5), S(100, -10), S(144, -20), S(147, -11), }

#define ROOK_SEMI_OPEN_FILE { \
S(1, 57), S(26, 24), S(26, 12), S(31, 7), S(32, -3), S(39, -9), S(59, -11), S(33, 37), }

#define QUEEN_OPEN_FILE { \
S(6, 36), S(-1, 44), S(-17, 42), S(-18, 57), S(0, 48), S(35, 4), S(55, -17), S(103, -30), }

#define QUEEN_SEMI_OPEN_FILE { \
S(1, 10), S(16, 15), S(4, 6), S(-8, 48), S(13, 16), S(30, -38), S(52, -68), S(46, -38), }

#define KING_OPEN_FILE { \
S(-95, -14), S(-134, -2), S(-101, -3), S(-45, -13), S(-68, -6), S(-98, 4), S(-113, 8), S(-68, -1), }

#define KING_SEMI_OPEN_FILE { \
S(-43, 38), S(-75, 15), S(-30, -4), S(-17, -7), S(-17, -5), S(-39, -2), S(-58, 11), S(-57, 32), }

#define ISOLATED_PAWNS { \
S(153, -414), S(30, -128), S(2, -50), S(-8, -20), S(-2, -2), S(-19, -4), S(3, 30), S(20, 90), }

/*
Average PST values for MG, EG:
Knight Values: 454 360
Bishop Values: 459 398
Rook Values: 553 725
Queen Values: 1255 1311
Pawn Values: 110 162
*/

#define KNIGHT_PST { { \
   S(213, 223), S(416, 246), S(277, 304), S(458, 293), S(525, 284), S(432, 296), S(401, 288), S(343, 237), \
   S(414, 277), S(516, 291), S(461, 322), S(535, 318), S(476, 340), S(571, 304), S(421, 324), S(319, 326), \
   S(450, 294), S(536, 308), S(540, 327), S(538, 337), S(521, 339), S(528, 327), S(479, 342), S(369, 321), \
   S(446, 331), S(467, 341), S(524, 364), S(512, 358), S(522, 365), S(526, 346), S(478, 341), S(405, 333), \
   S(440, 314), S(489, 337), S(478, 373), S(485, 362), S(456, 377), S(470, 360), S(421, 346), S(376, 347), \
   S(417, 307), S(437, 349), S(443, 357), S(454, 370), S(481, 355), S(454, 346), S(431, 347), S(373, 338), \
   S(338, 302), S(404, 336), S(408, 350), S(415, 362), S(408, 357), S(419, 349), S(380, 334), S(355, 320), \
   S(400, 258), S(384, 291), S(330, 350), S(392, 329), S(345, 334), S(379, 317), S(399, 257), S(308, 299), \
}, { \
   S(253, 355), S(265, 416), S(396, 392), S(416, 389), S(493, 376), S(347, 394), S(412, 325), S(341, 265), \
   S(386, 382), S(420, 418), S(519, 398), S(544, 402), S(514, 410), S(562, 369), S(466, 372), S(456, 355), \
   S(413, 397), S(507, 397), S(532, 429), S(570, 420), S(620, 400), S(692, 375), S(575, 390), S(503, 363), \
   S(464, 396), S(499, 425), S(517, 439), S(559, 442), S(528, 448), S(584, 438), S(496, 440), S(529, 392), \
   S(455, 397), S(473, 429), S(495, 444), S(499, 448), S(512, 451), S(505, 444), S(510, 420), S(472, 388), \
   S(449, 384), S(469, 411), S(495, 414), S(488, 435), S(508, 439), S(500, 415), S(507, 407), S(454, 392), \
   S(432, 345), S(442, 372), S(465, 402), S(476, 406), S(480, 408), S(494, 393), S(485, 380), S(460, 365), \
   S(336, 361), S(448, 336), S(415, 379), S(431, 394), S(451, 393), S(452, 389), S(448, 361), S(421, 327), \
}, }

#define BISHOP_PST { { \
   S(313, 368), S(436, 350), S(420, 337), S(369, 341), S(255, 382), S(308, 371), S(366, 359), S(391, 358), \
   S(459, 324), S(463, 351), S(421, 361), S(430, 354), S(511, 357), S(407, 375), S(406, 390), S(298, 383), \
   S(479, 347), S(610, 321), S(487, 367), S(525, 361), S(494, 374), S(510, 364), S(409, 386), S(396, 371), \
   S(462, 355), S(442, 373), S(502, 377), S(520, 368), S(494, 396), S(489, 378), S(475, 370), S(405, 368), \
   S(418, 358), S(438, 376), S(426, 390), S(470, 388), S(486, 378), S(499, 369), S(449, 366), S(388, 367), \
   S(420, 354), S(429, 374), S(438, 386), S(438, 383), S(476, 379), S(465, 371), S(488, 365), S(442, 368), \
   S(436, 341), S(431, 373), S(431, 356), S(436, 363), S(437, 365), S(475, 361), S(477, 348), S(404, 345), \
   S(300, 362), S(350, 379), S(413, 339), S(372, 375), S(403, 358), S(405, 342), S(410, 361), S(353, 361), \
}, { \
   S(467, 415), S(428, 425), S(403, 433), S(388, 433), S(411, 430), S(422, 427), S(433, 423), S(465, 397), \
   S(429, 439), S(495, 438), S(487, 433), S(461, 431), S(505, 430), S(522, 413), S(505, 426), S(435, 406), \
   S(443, 440), S(490, 447), S(513, 448), S(523, 440), S(546, 434), S(580, 446), S(532, 437), S(493, 425), \
   S(464, 437), S(485, 451), S(506, 458), S(555, 454), S(532, 466), S(528, 456), S(491, 449), S(471, 432), \
   S(470, 427), S(493, 445), S(498, 461), S(520, 466), S(520, 459), S(499, 454), S(501, 432), S(480, 417), \
   S(489, 420), S(504, 439), S(499, 452), S(505, 451), S(497, 458), S(511, 443), S(498, 431), S(490, 415), \
   S(502, 401), S(506, 400), S(506, 417), S(480, 431), S(492, 434), S(508, 429), S(528, 409), S(492, 390), \
   S(452, 396), S(481, 411), S(465, 397), S(449, 426), S(461, 422), S(464, 412), S(469, 409), S(464, 390), \
}, }

#define ROOK_PST { { \
   S(523, 678), S(491, 689), S(501, 687), S(529, 682), S(534, 684), S(534, 686), S(500, 685), S(507, 673), \
   S(584, 647), S(584, 656), S(593, 649), S(579, 650), S(523, 667), S(535, 678), S(448, 688), S(449, 683), \
   S(563, 650), S(581, 650), S(607, 654), S(590, 636), S(539, 655), S(482, 687), S(484, 672), S(433, 667), \
   S(516, 657), S(476, 669), S(482, 672), S(531, 664), S(488, 677), S(514, 681), S(415, 687), S(451, 664), \
   S(446, 663), S(476, 664), S(472, 669), S(476, 665), S(470, 670), S(474, 678), S(419, 675), S(422, 669), \
   S(437, 655), S(466, 659), S(493, 655), S(491, 642), S(458, 661), S(477, 668), S(420, 676), S(446, 643), \
   S(459, 637), S(455, 661), S(481, 647), S(476, 651), S(449, 665), S(452, 680), S(402, 662), S(416, 652), \
   S(491, 646), S(501, 657), S(494, 665), S(492, 671), S(495, 667), S(485, 675), S(414, 693), S(464, 654), \
}, { \
   S(656, 781), S(654, 792), S(615, 810), S(669, 785), S(667, 792), S(634, 807), S(664, 800), S(697, 779), \
   S(624, 795), S(617, 810), S(654, 802), S(695, 783), S(692, 780), S(692, 790), S(572, 823), S(620, 792), \
   S(588, 790), S(615, 798), S(626, 798), S(653, 782), S(661, 777), S(697, 773), S(664, 784), S(643, 771), \
   S(573, 792), S(592, 793), S(607, 806), S(616, 788), S(611, 788), S(646, 786), S(620, 791), S(602, 779), \
   S(584, 773), S(559, 798), S(579, 802), S(591, 787), S(607, 773), S(601, 782), S(602, 785), S(608, 754), \
   S(577, 765), S(574, 782), S(580, 787), S(584, 780), S(604, 772), S(609, 774), S(627, 764), S(598, 749), \
   S(573, 767), S(582, 770), S(579, 785), S(587, 778), S(599, 768), S(624, 770), S(636, 757), S(544, 759), \
   S(606, 776), S(596, 782), S(602, 794), S(612, 785), S(616, 780), S(620, 782), S(576, 792), S(623, 732), \
}, }

#define QUEEN_PST { { \
   S(1162, 1185), S(1164, 1197), S(1363, 1107), S(1136, 1193), S(1219, 1186), S(1152, 1211), S(1256, 1118), S(1050, 1209), \
   S(1328, 1088), S(1261, 1136), S(1347, 1130), S(1161, 1241), S(1231, 1201), S(1154, 1209), S(1054, 1247), S(1115, 1162), \
   S(1281, 1100), S(1302, 1142), S(1405, 1147), S(1294, 1146), S(1173, 1227), S(1129, 1236), S(1114, 1224), S(1071, 1198), \
   S(1187, 1190), S(1204, 1170), S(1168, 1212), S(1170, 1219), S(1173, 1183), S(1146, 1205), S(1109, 1217), S(1139, 1145), \
   S(1177, 1186), S(1157, 1210), S(1142, 1284), S(1176, 1201), S(1151, 1166), S(1121, 1250), S(1109, 1211), S(1087, 1170), \
   S(1175, 1149), S(1165, 1170), S(1150, 1191), S(1145, 1155), S(1150, 1183), S(1180, 1155), S(1120, 1225), S(1095, 1140), \
   S(1151, 1127), S(1135, 1155), S(1191, 1121), S(1141, 1167), S(1171, 1129), S(1185, 1135), S(1072, 1210), S(1102, 1114), \
   S(1088, 1178), S(1126, 1153), S(1133, 1162), S(1170, 1082), S(1155, 1109), S(1115, 1174), S(1057, 1202), S(1055, 1160), \
}, { \
   S(1287, 1427), S(1313, 1453), S(1330, 1462), S(1357, 1458), S(1393, 1448), S(1442, 1450), S(1439, 1461), S(1363, 1503), \
   S(1308, 1413), S(1297, 1442), S(1323, 1482), S(1326, 1484), S(1297, 1559), S(1425, 1494), S(1363, 1531), S(1355, 1512), \
   S(1310, 1406), S(1331, 1412), S(1335, 1457), S(1368, 1468), S(1406, 1466), S(1461, 1495), S(1427, 1480), S(1405, 1490), \
   S(1295, 1433), S(1311, 1449), S(1327, 1461), S(1344, 1463), S(1327, 1533), S(1348, 1557), S(1338, 1563), S(1339, 1507), \
   S(1334, 1394), S(1311, 1455), S(1337, 1450), S(1336, 1478), S(1337, 1484), S(1332, 1504), S(1343, 1509), S(1344, 1486), \
   S(1326, 1390), S(1351, 1375), S(1331, 1449), S(1345, 1420), S(1348, 1422), S(1345, 1461), S(1358, 1459), S(1343, 1434), \
   S(1316, 1372), S(1342, 1393), S(1366, 1367), S(1359, 1386), S(1358, 1385), S(1363, 1391), S(1355, 1393), S(1322, 1427), \
   S(1328, 1382), S(1314, 1391), S(1337, 1381), S(1363, 1343), S(1338, 1369), S(1292, 1415), S(1286, 1430), S(1315, 1359), \
}, }

#define PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(26, 323), S(39, 269), S(9, 280), S(25, 297), S(61, 334), S(10, 349), S(-27, 364), S(21, 360), \
   S(121, 147), S(238, 110), S(208, 128), S(288, 133), S(139, 182), S(164, 132), S(114, 185), S(121, 153), \
   S(58, 143), S(119, 128), S(136, 110), S(150, 110), S(149, 120), S(149, 122), S(151, 144), S(120, 134), \
   S(39, 138), S(81, 136), S(114, 111), S(138, 113), S(135, 115), S(143, 115), S(129, 133), S(109, 121), \
   S(33, 130), S(77, 136), S(99, 117), S(90, 125), S(118, 121), S(131, 117), S(153, 120), S(113, 111), \
   S(27, 142), S(84, 138), S(109, 117), S(87, 115), S(101, 121), S(163, 115), S(159, 121), S(97, 114), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(82, 353), S(93, 336), S(38, 346), S(78, 309), S(103, 285), S(43, 274), S(39, 297), S(-25, 335), \
   S(82, 183), S(137, 164), S(155, 140), S(165, 143), S(220, 146), S(227, 126), S(201, 164), S(128, 150), \
   S(86, 162), S(135, 148), S(123, 129), S(143, 107), S(153, 111), S(146, 112), S(134, 132), S(92, 127), \
   S(67, 150), S(119, 142), S(114, 118), S(137, 116), S(136, 115), S(137, 110), S(122, 125), S(85, 118), \
   S(74, 136), S(115, 133), S(113, 117), S(103, 127), S(115, 133), S(127, 118), S(154, 117), S(99, 110), \
   S(61, 145), S(120, 134), S(89, 135), S(81, 138), S(86, 145), S(154, 125), S(161, 123), S(89, 110), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define KING_PST { { \
   S(83, -101), S(139, -34), S(85, -73), S(210, -81), S(323, -32), S(394, -11), S(433, -46), S(344, -59), \
   S(64, -41), S(212, 21), S(152, 9), S(-10, 19), S(335, 38), S(419, 28), S(430, 15), S(500, -49), \
   S(113, -18), S(134, 10), S(258, 22), S(-23, 36), S(356, 42), S(460, 33), S(444, 16), S(416, -5), \
   S(-53, -22), S(48, 18), S(33, 29), S(-99, 60), S(313, 60), S(299, 60), S(381, 45), S(366, 8), \
   S(-113, -9), S(-65, 13), S(-98, 44), S(-179, 67), S(234, 77), S(334, 48), S(362, 21), S(291, 0), \
   S(-124, -20), S(-27, 1), S(-113, 38), S(-149, 53), S(301, 55), S(292, 40), S(372, 8), S(382, -21), \
   S(14, -43), S(11, -13), S(-28, 5), S(-131, 29), S(319, 35), S(352, 13), S(403, -14), S(401, -37), \
   S(16, -94), S(52, -69), S(-30, -34), S(-133, -3), S(363, -24), S(312, -13), S(426, -58), S(412, -86), \
}, { \
   S(-182, -150), S(-235, -64), S(-241, -53), S(-332, -28), S(-33, -21), S(34, -20), S(313, -86), S(92, -65), \
   S(-162, -64), S(-253, 0), S(-256, 0), S(-394, 26), S(63, 14), S(77, 18), S(117, 21), S(-53, 20), \
   S(-308, -8), S(-252, 10), S(-313, 19), S(-367, 43), S(55, 36), S(262, 27), S(189, 44), S(26, 8), \
   S(-337, -7), S(-370, 33), S(-386, 38), S(-443, 62), S(-50, 56), S(31, 43), S(-75, 48), S(-202, 23), \
   S(-447, -6), S(-326, -1), S(-424, 39), S(-517, 64), S(-118, 73), S(-113, 58), S(-90, 31), S(-155, 4), \
   S(-384, -27), S(-379, 0), S(-434, 27), S(-511, 55), S(-108, 65), S(-76, 44), S(-18, 13), S(-47, -15), \
   S(-287, -74), S(-338, -23), S(-414, 12), S(-474, 31), S(-108, 50), S(-45, 27), S(27, -4), S(30, -35), \
   S(-310, -111), S(-271, -91), S(-311, -60), S(-458, -8), S(-13, -25), S(-69, -4), S(47, -47), S(42, -92), \
}, }

#endif