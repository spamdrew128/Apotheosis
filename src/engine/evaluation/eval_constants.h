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
   S(28, 71)

#define PASSED_PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(76, 209), S(15, 190), S(55, 190), S(149, 186), S(38, 262), S(48, 231), S(18, 272), S(54, 269), \
   S(51, 165), S(-65, 193), S(-40, 146), S(-42, 108), S(45, 146), S(-85, 238), S(-74, 275), S(-74, 291), \
   S(2, 87), S(3, 71), S(105, 36), S(0, 50), S(-1, 70), S(-61, 115), S(-119, 165), S(-116, 188), \
   S(-9, 53), S(-34, 50), S(35, 26), S(-10, 27), S(-78, 57), S(-71, 66), S(-50, 91), S(-59, 91), \
   S(81, 9), S(-26, 35), S(-12, 17), S(-6, 23), S(-58, 26), S(-11, 10), S(-28, 37), S(-30, 45), \
   S(6, 11), S(82, 25), S(94, 0), S(-32, 17), S(-19, 17), S(-48, 25), S(-39, 27), S(-42, 33), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(101, 259), S(90, 255), S(78, 247), S(113, 205), S(81, 209), S(74, 179), S(15, 212), S(-6, 242), \
   S(52, 230), S(17, 240), S(24, 206), S(25, 164), S(-8, 137), S(61, 110), S(-55, 129), S(-44, 154), \
   S(28, 130), S(-4, 128), S(10, 101), S(11, 86), S(15, 56), S(67, 42), S(6, 84), S(20, 73), \
   S(5, 73), S(-23, 74), S(-38, 60), S(-19, 46), S(-23, 41), S(-5, 40), S(14, 56), S(28, 48), \
   S(1, 22), S(-13, 39), S(-31, 32), S(-29, 28), S(0, 12), S(34, 13), S(35, 39), S(56, 13), \
   S(-6, 20), S(8, 27), S(7, 15), S(-25, 29), S(-1, 14), S(12, 14), S(58, 31), S(7, 30), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define BLOCKED_PASSERS { \
   S(67, -202), S(8, -121), S(-8, -47), S(1, -25), S(-19, 5), S(4, -15), S(0, 0), S(0, 0), \
}

#define ROOK_OPEN_FILE { \
   S(30, 0), S(36, 0), S(36, 1), S(40, 0), S(41, 5), S(36, -4), S(108, -21), S(99, -11), \
}

#define ROOK_SEMI_OPEN_FILE { \
   S(-11, 43), S(4, 13), S(12, 7), S(23, -3), S(17, -7), S(10, -7), S(38, -10), S(19, 23), \
}

#define QUEEN_OPEN_FILE { \
   S(-13, 11), S(-7, 27), S(-19, 22), S(-19, 34), S(-13, 40), S(-9, 13), S(22, 15), S(40, -15), \
}

#define QUEEN_SEMI_OPEN_FILE { \
   S(-7, 2), S(5, 0), S(5, 0), S(-4, 28), S(-1, 27), S(15, -38), S(50, -51), S(27, -20), \
}

#define KING_OPEN_FILE { \
   S(-65, -15), S(-118, 8), S(-99, 4), S(-42, -12), S(-73, -3), S(-103, 9), S(-107, 19), S(-69, 10), \
}

#define KING_SEMI_OPEN_FILE { \
   S(-37, 33), S(-61, 17), S(-19, -4), S(3, -8), S(-14, 0), S(-32, 1), S(-41, 19), S(-42, 33), \
}

#define ISOLATED_PAWNS { \
   S(-14, -17), S(-18, -16), S(-18, -21), S(-32, -15), S(-30, -22), S(-9, -19), S(-16, -15), S(-27, -19), \
}

#define KNIGHT_MOBILITY { \
   S(246, 171), S(265, 238), S(276, 261), S(282, 273), S(296, 278), S(303, 289), S(314, 286), S(318, 286), S(341, 264), \
}

#define BISHOP_MOBILITY { \
   S(237, 190), S(252, 213), S(266, 232), S(274, 251), S(282, 264), S(286, 274), S(291, 282), S(295, 283), S(292, 290), S(300, 287), S(316, 281), S(344, 277), S(267, 316), S(350, 268), \
}

#define ROOK_MOBILITY { \
   S(287, 351), S(301, 377), S(307, 394), S(311, 407), S(311, 419), S(317, 425), S(320, 432), S(327, 435), S(337, 440), S(343, 445), S(344, 451), S(353, 454), S(350, 462), S(365, 456), S(367, 454), \
}

#define QUEEN_MOBILITY { \
   S(729, 716), S(725, 627), S(722, 609), S(721, 634), S(726, 672), S(728, 691), S(730, 698), S(729, 736), S(731, 748), S(735, 755), S(740, 766), S(741, 780), S(737, 789), S(747, 792), S(748, 805), S(745, 818), S(743, 825), S(758, 813), S(775, 814), S(779, 812), S(771, 829), S(826, 799), S(792, 815), S(894, 763), S(964, 715), S(1053, 680), S(796, 860), S(715, 860), \
}

#define KNIGHT_KING_ZONE_ATTACKS { \
   S(0, 0), S(0, 7), S(6, 8), S(32, -10), S(98, -10), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}

#define BISHOP_KING_ZONE_ATTACKS { \
   S(0, 0), S(5, 7), S(10, 2), S(30, 3), S(68, -7), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}

#define ROOK_KING_ZONE_ATTACKS { \
   S(0, 0), S(10, -6), S(18, -15), S(26, -10), S(58, -30), S(91, -29), S(118, -34), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}

#define QUEEN_KING_ZONE_ATTACKS { \
   S(0, 0), S(4, 9), S(4, 21), S(5, 39), S(4, 41), S(24, 50), S(35, 51), S(88, 39), S(128, 27), S(244, -43), S(320, -49), S(0, 0), S(0, 0), S(0, 0), \
}

#define PAWN_KING_ZONE_ATTACKS { \
   S(0, 0), S(2, -20), S(8, -25), S(23, -28), S(31, -24), S(52, -24), S(89, -31), S(-127, 52), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}

#define CHECK_BONUS { \
   S(123, 3), S(127, 65), S(201, -9), S(100, 118), S(364, -9), \
}

#define PAWN_THREAT_ON_KNIGHT S(60, 11)
#define PAWN_THREAT_ON_BISHOP S(60, 41)
#define PAWN_THREAT_ON_ROOK S(118, -38)
#define PAWN_THREAT_ON_QUEEN S(62, -6)

#define KNIGHT_THREAT_ON_BISHOP S(27, 24)
#define KNIGHT_THREAT_ON_ROOK S(63, -10)
#define KNIGHT_THREAT_ON_QUEEN S(27, -32)

#define BISHOP_THREAT_ON_KNIGHT S(24, 30)
#define BISHOP_THREAT_ON_ROOK S(44, 5)
#define BISHOP_THREAT_ON_QUEEN S(45, 37)

#define ROOK_THREAT_ON_QUEEN S(61, -20)

/*
Average PST values for MG, EG:
Knight Values: 143 122
Bishop Values: 159 158
Rook Values: 222 350
Queen Values: 537 591
Pawn Values: 101 144
*/

#define KNIGHT_PST { { \
   S(-81, 9), S(67, 13), S(-1, 118), S(163, 51), S(191, 72), S(180, 48), S(121, 41), S(79, 49), \
   S(103, 61), S(186, 60), S(192, 76), S(170, 80), S(92, 103), S(152, 96), S(121, 95), S(29, 109), \
   S(41, 97), S(178, 81), S(173, 112), S(186, 116), S(148, 121), S(146, 105), S(103, 111), S(120, 84), \
   S(156, 84), S(168, 101), S(169, 132), S(183, 137), S(197, 120), S(181, 121), S(161, 105), S(85, 102), \
   S(145, 100), S(180, 90), S(163, 132), S(173, 128), S(152, 131), S(144, 124), S(141, 111), S(111, 118), \
   S(130, 76), S(140, 103), S(138, 107), S(153, 132), S(188, 112), S(148, 102), S(149, 81), S(110, 91), \
   S(79, 86), S(169, 96), S(111, 104), S(129, 113), S(131, 106), S(134, 92), S(94, 92), S(67, 110), \
   S(21, 82), S(114, 87), S(93, 112), S(104, 109), S(105, 94), S(125, 82), S(105, 90), S(74, 107), \
}, { \
   S(-16, 128), S(25, 138), S(65, 161), S(135, 123), S(177, 133), S(11, 149), S(59, 100), S(37, -5), \
   S(46, 161), S(94, 178), S(177, 138), S(167, 163), S(161, 157), S(202, 129), S(159, 127), S(156, 95), \
   S(103, 145), S(187, 147), S(187, 180), S(206, 171), S(209, 166), S(281, 157), S(208, 127), S(148, 132), \
   S(160, 160), S(176, 173), S(173, 196), S(229, 186), S(191, 195), S(230, 184), S(184, 175), S(206, 145), \
   S(151, 163), S(161, 159), S(177, 187), S(191, 188), S(196, 187), S(188, 185), S(214, 158), S(169, 151), \
   S(143, 143), S(152, 160), S(176, 156), S(179, 168), S(199, 174), S(188, 147), S(186, 139), S(159, 155), \
   S(133, 125), S(136, 143), S(155, 143), S(183, 145), S(183, 156), S(183, 140), S(184, 126), S(169, 129), \
   S(55, 144), S(153, 123), S(150, 132), S(151, 151), S(173, 147), S(171, 141), S(159, 134), S(165, 87), \
}, }

#define BISHOP_PST { { \
   S(156, 133), S(116, 121), S(26, 141), S(-75, 136), S(-51, 157), S(-5, 134), S(43, 153), S(113, 134), \
   S(139, 111), S(141, 120), S(149, 118), S(132, 82), S(189, 121), S(106, 150), S(75, 154), S(19, 161), \
   S(208, 104), S(238, 103), S(184, 148), S(197, 127), S(159, 134), S(194, 111), S(132, 151), S(161, 140), \
   S(161, 110), S(139, 138), S(161, 142), S(213, 139), S(160, 151), S(170, 138), S(196, 122), S(107, 151), \
   S(136, 119), S(130, 135), S(120, 145), S(163, 142), S(193, 131), S(175, 127), S(133, 134), S(127, 149), \
   S(131, 134), S(137, 131), S(154, 137), S(150, 143), S(179, 138), S(189, 128), S(181, 148), S(158, 142), \
   S(150, 108), S(150, 132), S(188, 116), S(155, 134), S(166, 124), S(197, 107), S(210, 102), S(126, 150), \
   S(62, 138), S(74, 168), S(152, 145), S(157, 144), S(184, 124), S(159, 125), S(165, 125), S(147, 120), \
}, { \
   S(156, 184), S(131, 176), S(73, 192), S(34, 203), S(110, 181), S(69, 194), S(207, 167), S(166, 155), \
   S(140, 195), S(154, 197), S(130, 186), S(97, 200), S(153, 180), S(170, 178), S(143, 183), S(102, 170), \
   S(163, 192), S(199, 186), S(196, 182), S(191, 177), S(189, 180), S(233, 193), S(189, 188), S(203, 177), \
   S(159, 190), S(186, 189), S(184, 201), S(226, 201), S(192, 204), S(206, 195), S(174, 182), S(173, 189), \
   S(169, 186), S(182, 191), S(189, 198), S(211, 204), S(203, 194), S(180, 193), S(175, 188), S(174, 171), \
   S(185, 175), S(201, 188), S(198, 195), S(195, 190), S(202, 199), S(208, 186), S(197, 177), S(187, 173), \
   S(194, 172), S(207, 158), S(204, 169), S(196, 179), S(204, 179), S(222, 172), S(229, 167), S(204, 156), \
   S(178, 164), S(208, 170), S(189, 185), S(177, 181), S(201, 179), S(184, 194), S(205, 175), S(197, 160), \
}, }

#define ROOK_PST { { \
   S(240, 308), S(167, 316), S(201, 311), S(144, 336), S(152, 338), S(156, 339), S(198, 319), S(120, 331), \
   S(286, 283), S(305, 284), S(197, 303), S(216, 305), S(227, 296), S(176, 323), S(128, 327), S(121, 327), \
   S(239, 294), S(254, 295), S(221, 292), S(254, 291), S(179, 310), S(172, 319), S(236, 293), S(100, 312), \
   S(176, 303), S(177, 312), S(170, 324), S(164, 321), S(186, 310), S(225, 314), S(103, 328), S(120, 317), \
   S(125, 307), S(124, 325), S(120, 329), S(162, 321), S(192, 303), S(166, 323), S(161, 301), S(155, 290), \
   S(152, 293), S(139, 309), S(175, 291), S(195, 272), S(135, 310), S(179, 303), S(136, 305), S(137, 285), \
   S(160, 285), S(149, 304), S(187, 283), S(173, 292), S(183, 294), S(184, 303), S(120, 302), S(135, 290), \
   S(185, 280), S(197, 288), S(179, 296), S(184, 295), S(199, 283), S(189, 298), S(135, 307), S(158, 290), \
}, { \
   S(286, 400), S(314, 395), S(250, 425), S(305, 400), S(290, 403), S(226, 432), S(293, 408), S(304, 396), \
   S(263, 410), S(264, 417), S(297, 411), S(322, 401), S(319, 390), S(322, 400), S(197, 441), S(259, 405), \
   S(244, 403), S(277, 406), S(265, 411), S(278, 408), S(290, 389), S(332, 384), S(302, 408), S(265, 391), \
   S(236, 404), S(277, 392), S(265, 411), S(263, 400), S(255, 394), S(252, 408), S(242, 400), S(235, 395), \
   S(240, 396), S(247, 400), S(255, 407), S(262, 398), S(286, 377), S(249, 396), S(262, 399), S(242, 386), \
   S(249, 381), S(257, 391), S(261, 386), S(252, 392), S(269, 386), S(287, 381), S(282, 384), S(258, 365), \
   S(239, 382), S(261, 378), S(261, 384), S(269, 389), S(279, 374), S(285, 379), S(282, 373), S(210, 381), \
   S(270, 381), S(271, 380), S(273, 386), S(283, 378), S(287, 375), S(290, 381), S(247, 387), S(272, 348), \
}, }

#define QUEEN_PST { { \
   S(492, 549), S(563, 492), S(719, 403), S(542, 457), S(631, 429), S(512, 504), S(523, 437), S(416, 541), \
   S(639, 425), S(525, 484), S(628, 410), S(503, 510), S(439, 522), S(438, 522), S(427, 502), S(434, 503), \
   S(558, 450), S(485, 519), S(558, 494), S(477, 517), S(493, 517), S(482, 507), S(435, 504), S(430, 510), \
   S(490, 503), S(463, 493), S(377, 542), S(452, 533), S(437, 489), S(469, 510), S(391, 541), S(464, 529), \
   S(495, 495), S(434, 538), S(433, 548), S(443, 526), S(430, 492), S(443, 571), S(424, 538), S(412, 547), \
   S(522, 443), S(478, 439), S(472, 488), S(475, 438), S(461, 492), S(504, 464), S(450, 518), S(435, 517), \
   S(447, 481), S(461, 428), S(500, 431), S(484, 465), S(498, 468), S(525, 463), S(427, 506), S(445, 438), \
   S(418, 487), S(443, 422), S(452, 461), S(491, 422), S(503, 431), S(460, 442), S(394, 497), S(391, 530), \
}, { \
   S(555, 689), S(574, 693), S(537, 748), S(606, 696), S(696, 634), S(578, 749), S(711, 670), S(635, 751), \
   S(556, 696), S(541, 705), S(576, 739), S(561, 762), S(508, 805), S(606, 708), S(620, 753), S(672, 732), \
   S(595, 668), S(590, 666), S(578, 696), S(575, 741), S(609, 698), S(630, 740), S(590, 710), S(609, 745), \
   S(563, 713), S(581, 696), S(586, 695), S(565, 725), S(564, 739), S(587, 731), S(567, 761), S(595, 710), \
   S(599, 669), S(574, 703), S(590, 705), S(582, 728), S(593, 697), S(577, 712), S(600, 705), S(595, 727), \
   S(588, 694), S(608, 660), S(600, 697), S(606, 669), S(606, 686), S(612, 686), S(616, 689), S(603, 709), \
   S(593, 656), S(604, 656), S(619, 643), S(629, 631), S(630, 641), S(628, 655), S(612, 626), S(617, 657), \
   S(609, 654), S(594, 644), S(606, 652), S(628, 622), S(612, 649), S(587, 674), S(586, 650), S(587, 615), \
}, }

#define PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(76, 209), S(15, 190), S(55, 190), S(149, 186), S(38, 262), S(48, 231), S(18, 272), S(54, 269), \
   S(85, 152), S(203, 106), S(228, 113), S(219, 132), S(65, 166), S(168, 125), S(97, 166), S(114, 163), \
   S(50, 138), S(57, 130), S(102, 124), S(113, 125), S(110, 132), S(109, 126), S(149, 120), S(150, 130), \
   S(29, 133), S(40, 135), S(90, 123), S(117, 119), S(117, 118), S(112, 115), S(115, 113), S(118, 120), \
   S(21, 128), S(44, 127), S(82, 123), S(72, 132), S(107, 121), S(115, 119), S(132, 96), S(135, 103), \
   S(27, 137), S(51, 129), S(91, 123), S(93, 124), S(102, 125), S(140, 114), S(143, 100), S(123, 109), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(101, 259), S(90, 255), S(78, 247), S(113, 205), S(81, 209), S(74, 179), S(15, 212), S(-6, 242), \
   S(82, 182), S(88, 168), S(110, 157), S(136, 134), S(189, 128), S(184, 132), S(198, 144), S(155, 156), \
   S(76, 158), S(99, 136), S(102, 131), S(118, 103), S(117, 128), S(118, 120), S(91, 128), S(95, 132), \
   S(66, 143), S(88, 127), S(100, 123), S(127, 112), S(118, 123), S(119, 113), S(87, 115), S(89, 120), \
   S(70, 130), S(91, 116), S(100, 117), S(108, 124), S(116, 134), S(117, 118), S(121, 103), S(103, 111), \
   S(71, 136), S(100, 113), S(87, 134), S(108, 126), S(107, 147), S(145, 120), S(139, 103), S(102, 104), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define KING_PST { { \
   S(-87, -81), S(-2, -12), S(32, -57), S(-198, 14), S(22, 40), S(-6, 68), S(271, 5), S(336, -46), \
   S(172, -76), S(121, 21), S(194, 6), S(113, 14), S(223, 46), S(222, 62), S(264, 36), S(308, -11), \
   S(-19, 21), S(11, 29), S(51, 51), S(29, 49), S(245, 67), S(325, 58), S(386, 34), S(257, 12), \
   S(-69, -8), S(-7, 35), S(-6, 43), S(-123, 65), S(248, 63), S(231, 61), S(365, 32), S(295, 10), \
   S(-153, 2), S(-33, 1), S(-18, 35), S(-125, 59), S(186, 63), S(255, 43), S(272, 15), S(198, -6), \
   S(-12, -36), S(6, -7), S(-49, 22), S(-25, 29), S(224, 45), S(259, 22), S(282, -2), S(310, -25), \
   S(42, -47), S(51, -33), S(26, -10), S(-54, 15), S(275, 22), S(268, 1), S(312, -26), S(338, -57), \
   S(59, -95), S(73, -64), S(25, -46), S(-60, -9), S(289, -23), S(254, -19), S(332, -59), S(319, -99), \
}, { \
   S(-204, -125), S(-57, -97), S(-62, -49), S(-336, 9), S(-92, 24), S(-5, 5), S(137, -16), S(-48, -25), \
   S(-259, 7), S(-212, 19), S(-323, 38), S(-292, 45), S(-3, 55), S(11, 56), S(142, 13), S(-205, 59), \
   S(-319, 19), S(-200, 24), S(-177, 22), S(-328, 52), S(36, 51), S(260, 41), S(219, 39), S(29, 7), \
   S(-222, -1), S(-223, 25), S(-284, 44), S(-344, 64), S(-112, 77), S(-46, 63), S(-57, 44), S(-176, 22), \
   S(-412, 13), S(-249, 0), S(-303, 34), S(-429, 68), S(-161, 91), S(-86, 57), S(-87, 29), S(-114, 4), \
   S(-279, -28), S(-302, 0), S(-322, 21), S(-408, 51), S(-61, 57), S(-48, 40), S(-13, 13), S(-24, -17), \
   S(-180, -80), S(-249, -29), S(-303, 5), S(-352, 24), S(-63, 39), S(-27, 24), S(19, -5), S(33, -38), \
   S(-231, -125), S(-204, -90), S(-227, -57), S(-378, 1), S(-12, -15), S(-52, 2), S(35, -40), S(28, -81), \
}, }

#endif