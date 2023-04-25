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
   S(30, 70)

#define PASSED_PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(95, 218), S(35, 198), S(57, 197), S(155, 194), S(44, 274), S(57, 244), S(31, 283), S(69, 280), \
   S(52, 166), S(-44, 184), S(-57, 146), S(-41, 99), S(73, 133), S(-69, 232), S(-58, 272), S(-74, 286), \
   S(0, 87), S(-2, 68), S(86, 38), S(-2, 47), S(4, 69), S(-57, 113), S(-107, 160), S(-111, 186), \
   S(-24, 55), S(-50, 49), S(8, 28), S(-22, 29), S(-75, 56), S(-69, 61), S(-40, 82), S(-52, 86), \
   S(63, 12), S(-29, 30), S(-29, 17), S(-15, 21), S(-56, 21), S(-5, 2), S(-35, 32), S(-34, 43), \
   S(2, 11), S(58, 24), S(58, 1), S(-61, 18), S(-6, 13), S(-49, 18), S(-52, 24), S(-35, 27), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(112, 271), S(98, 268), S(85, 261), S(120, 217), S(93, 217), S(83, 188), S(29, 219), S(7, 252), \
   S(54, 231), S(22, 238), S(23, 205), S(35, 155), S(-13, 129), S(64, 107), S(-53, 129), S(-41, 153), \
   S(27, 129), S(-4, 125), S(12, 97), S(13, 83), S(15, 58), S(63, 38), S(-1, 82), S(17, 73), \
   S(4, 71), S(-17, 69), S(-35, 56), S(-16, 44), S(-26, 43), S(-13, 36), S(8, 52), S(25, 46), \
   S(0, 20), S(-10, 34), S(-29, 28), S(-28, 25), S(-5, 10), S(26, 10), S(14, 39), S(44, 13), \
   S(-6, 18), S(7, 23), S(6, 11), S(-25, 25), S(-5, 8), S(0, 12), S(26, 32), S(0, 30), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define TEMPO_BONUS { \
   S(41, 31), \
}

#define BLOCKED_PASSERS { \
   S(62, -229), S(9, -124), S(-4, -50), S(6, -27), S(-14, 5), S(1, -11), S(0, 0), S(0, 0), \
}

#define ROOK_OPEN_FILE { \
   S(35, 0), S(36, -1), S(34, 0), S(40, 0), S(41, 5), S(43, -5), S(115, -25), S(112, -15), \
}

#define ROOK_SEMI_OPEN_FILE { \
   S(-6, 48), S(3, 18), S(10, 11), S(22, 0), S(16, -1), S(12, -1), S(36, -5), S(22, 26), \
}

#define QUEEN_OPEN_FILE { \
   S(-14, 7), S(-9, 31), S(-17, 24), S(-22, 39), S(-16, 41), S(-13, 13), S(17, 19), S(38, -18), \
}

#define QUEEN_SEMI_OPEN_FILE { \
   S(-4, 6), S(6, 6), S(6, 2), S(-6, 34), S(-2, 32), S(13, -27), S(42, -30), S(20, -14), \
}

#define KING_OPEN_FILE { \
   S(-37, -25), S(-100, 1), S(-80, 0), S(-21, -17), S(-57, -7), S(-71, 3), S(-86, 14), S(-35, 1), \
}

#define KING_SEMI_OPEN_FILE { \
   S(-6, 33), S(-46, 15), S(-6, -10), S(22, -17), S(-8, -4), S(-4, -6), S(-26, 16), S(-17, 32), \
}

#define ISOLATED_PAWNS { \
   S(-13, -17), S(-19, -17), S(-19, -21), S(-31, -17), S(-30, -23), S(-6, -21), S(-17, -15), S(-26, -20), \
}

#define KNIGHT_MOBILITY { \
   S(253, 191), S(274, 253), S(286, 274), S(293, 285), S(306, 290), S(313, 303), S(323, 302), S(326, 304), S(345, 284), \
}

#define BISHOP_MOBILITY { \
   S(248, 213), S(265, 232), S(280, 251), S(288, 269), S(296, 281), S(300, 291), S(305, 298), S(308, 298), S(305, 306), S(313, 301), S(327, 296), S(355, 292), S(290, 327), S(360, 281), \
}

#define ROOK_MOBILITY { \
   S(316, 372), S(329, 400), S(335, 416), S(340, 429), S(339, 442), S(346, 449), S(348, 457), S(355, 459), S(365, 465), S(370, 470), S(371, 475), S(380, 478), S(378, 485), S(394, 477), S(395, 473), \
}

#define QUEEN_MOBILITY { \
   S(772, 727), S(772, 671), S(768, 648), S(767, 682), S(771, 720), S(774, 739), S(777, 744), S(776, 786), S(778, 800), S(782, 810), S(786, 821), S(787, 839), S(784, 845), S(795, 849), S(796, 860), S(794, 873), S(793, 878), S(809, 863), S(820, 866), S(822, 864), S(820, 876), S(875, 845), S(832, 859), S(913, 819), S(988, 767), S(1088, 725), S(797, 922), S(701, 928), \
}

#define KNIGHT_KING_ZONE_ATTACKS { \
   S(-7, -13), S(-9, 2), S(-3, 10), S(0, 6), S(0, 8), S(7, 5), S(4, 11), S(7, 7), S(12, 5), S(13, 5), S(17, 3), S(18, 2), S(13, 3), S(20, 0), S(28, -2), S(18, 0), S(22, -2), S(26, -5), S(22, -8), S(20, -9), S(46, -15), S(36, -13), S(25, -16), S(19, -17), S(-10, -9), S(-71, -8), S(-6, -10), S(-105, -3), \
}

#define BISHOP_KING_ZONE_ATTACKS { \
   S(2, 7), S(0, 8), S(1, 8), S(4, 9), S(7, 10), S(9, 5), S(12, 0), S(10, 5), S(19, -1), S(14, 1), S(24, -4), S(20, -2), S(24, -2), S(24, -4), S(26, -2), S(20, -4), S(20, -3), S(23, -6), S(18, -2), S(27, -7), S(4, -1), S(13, -4), S(-14, 1), S(-31, 2), S(-42, 3), S(-21, -5), S(-6, -14), S(-74, 1), \
}

#define ROOK_KING_ZONE_ATTACKS { \
   S(9, -2), S(1, 0), S(7, -6), S(10, -12), S(10, -10), S(13, -13), S(12, -7), S(10, -6), S(17, -10), S(17, -10), S(23, -11), S(20, -11), S(21, -9), S(19, -8), S(28, -11), S(25, -8), S(24, -8), S(34, -11), S(33, -10), S(26, -8), S(19, -6), S(22, -7), S(28, -8), S(25, -7), S(27, -4), S(48, -12), S(4, 1), S(21, -6), \
}

#define QUEEN_KING_ZONE_ATTACKS { \
   S(-2, -5), S(0, -15), S(1, -15), S(5, -16), S(7, -12), S(4, -3), S(7, -5), S(8, 0), S(11, -4), S(14, 0), S(12, 4), S(19, 2), S(24, 0), S(20, 3), S(28, -4), S(42, -9), S(39, -8), S(60, -22), S(57, -18), S(64, -23), S(92, -39), S(89, -38), S(93, -34), S(141, -62), S(143, -56), S(147, -64), S(214, -89), S(50, -23), \
}

#define PAWN_KING_ZONE_ATTACKS { \
   S(8, 20), S(10, 3), S(10, -6), S(9, -3), S(9, -5), S(6, -3), S(7, -2), S(9, -2), S(5, -2), S(6, -3), S(7, -5), S(7, -4), S(15, -7), S(17, -7), S(14, -7), S(19, -8), S(25, -10), S(25, -9), S(22, -10), S(24, -10), S(36, -13), S(52, -16), S(43, -13), S(29, -12), S(26, -11), S(55, -10), S(42, -1), S(59, -1), \
}

#define CHECK_BONUS { \
   S(164, 39), S(153, 100), S(216, 22), S(104, 163), S(426, 19), \
}

#define PAWN_THREAT_ON_KNIGHT S(94, 36)
#define PAWN_THREAT_ON_BISHOP S(92, 63)
#define PAWN_THREAT_ON_ROOK S(145, -7)
#define PAWN_THREAT_ON_QUEEN S(104, 7)

#define KNIGHT_THREAT_ON_BISHOP S(42, 43)
#define KNIGHT_THREAT_ON_ROOK S(89, 18)
#define KNIGHT_THREAT_ON_QUEEN S(64, -17)

#define BISHOP_THREAT_ON_KNIGHT S(25, 38)
#define BISHOP_THREAT_ON_ROOK S(71, 37)
#define BISHOP_THREAT_ON_QUEEN S(84, 54)

#define ROOK_THREAT_ON_QUEEN S(96, 5)

/*
Average PST values for MG, EG:
Knight Values: 145 125
Bishop Values: 160 160
Rook Values: 227 350
Queen Values: 533 610
Pawn Values: 106 149
*/

#define KNIGHT_PST { { \
   S(-94, 6), S(100, 13), S(15, 113), S(137, 61), S(197, 67), S(170, 58), S(129, 42), S(97, 43), \
   S(84, 67), S(142, 70), S(132, 81), S(138, 85), S(79, 108), S(150, 104), S(119, 106), S(45, 106), \
   S(48, 100), S(162, 87), S(185, 101), S(183, 113), S(140, 125), S(136, 114), S(118, 112), S(154, 76), \
   S(148, 94), S(173, 107), S(176, 134), S(183, 142), S(195, 127), S(175, 131), S(169, 116), S(106, 100), \
   S(143, 113), S(181, 99), S(164, 140), S(177, 135), S(150, 140), S(144, 134), S(147, 123), S(117, 123), \
   S(126, 85), S(137, 110), S(134, 112), S(147, 144), S(181, 127), S(149, 109), S(154, 89), S(124, 90), \
   S(73, 95), S(175, 107), S(111, 114), S(123, 123), S(129, 116), S(126, 103), S(96, 97), S(68, 106), \
   S(50, 84), S(112, 92), S(88, 122), S(117, 116), S(109, 99), S(117, 91), S(101, 97), S(73, 111), \
}, { \
   S(-3, 120), S(37, 134), S(57, 167), S(110, 135), S(155, 137), S(-9, 145), S(51, 91), S(49, -9), \
   S(61, 162), S(106, 183), S(175, 145), S(158, 162), S(173, 136), S(192, 118), S(128, 134), S(163, 88), \
   S(116, 149), S(192, 150), S(200, 180), S(216, 164), S(224, 154), S(299, 143), S(223, 120), S(171, 125), \
   S(172, 162), S(186, 178), S(183, 199), S(234, 187), S(203, 190), S(243, 179), S(198, 172), S(219, 142), \
   S(158, 169), S(175, 163), S(187, 192), S(199, 189), S(207, 185), S(201, 184), S(225, 161), S(178, 150), \
   S(147, 150), S(159, 166), S(177, 162), S(187, 173), S(202, 178), S(191, 148), S(197, 137), S(161, 161), \
   S(140, 126), S(143, 147), S(161, 148), S(183, 154), S(182, 163), S(178, 145), S(183, 132), S(169, 132), \
   S(69, 140), S(152, 130), S(158, 135), S(156, 158), S(176, 151), S(164, 149), S(154, 144), S(175, 92), \
}, }

#define BISHOP_PST { { \
   S(147, 130), S(70, 132), S(49, 134), S(-99, 133), S(-56, 160), S(-14, 138), S(31, 155), S(114, 138), \
   S(148, 110), S(137, 120), S(129, 120), S(142, 80), S(168, 128), S(117, 156), S(92, 158), S(26, 161), \
   S(203, 108), S(233, 107), S(190, 150), S(202, 126), S(158, 138), S(190, 118), S(145, 155), S(171, 138), \
   S(162, 111), S(145, 141), S(158, 147), S(221, 142), S(166, 156), S(180, 141), S(197, 127), S(121, 148), \
   S(134, 123), S(126, 138), S(116, 151), S(162, 151), S(191, 140), S(171, 134), S(137, 134), S(139, 147), \
   S(121, 137), S(138, 135), S(158, 139), S(152, 146), S(173, 147), S(189, 133), S(185, 149), S(166, 136), \
   S(154, 107), S(151, 133), S(190, 120), S(153, 137), S(165, 129), S(193, 113), S(203, 105), S(128, 141), \
   S(73, 139), S(68, 180), S(151, 145), S(155, 148), S(182, 128), S(153, 131), S(144, 131), S(148, 123), \
}, { \
   S(160, 186), S(122, 181), S(68, 197), S(20, 204), S(106, 179), S(57, 189), S(193, 164), S(174, 154), \
   S(151, 194), S(168, 198), S(141, 188), S(107, 201), S(157, 175), S(172, 176), S(153, 177), S(96, 172), \
   S(170, 192), S(206, 187), S(204, 186), S(197, 179), S(204, 177), S(242, 190), S(213, 186), S(208, 174), \
   S(170, 189), S(189, 193), S(193, 203), S(236, 204), S(199, 206), S(208, 198), S(176, 182), S(178, 189), \
   S(177, 185), S(190, 191), S(195, 202), S(217, 207), S(211, 194), S(181, 192), S(174, 189), S(176, 168), \
   S(189, 177), S(206, 190), S(203, 198), S(199, 191), S(199, 201), S(211, 189), S(198, 178), S(189, 172), \
   S(196, 171), S(209, 159), S(207, 168), S(194, 181), S(202, 183), S(214, 179), S(225, 169), S(201, 157), \
   S(183, 165), S(215, 167), S(187, 185), S(181, 183), S(201, 179), S(175, 200), S(203, 174), S(202, 158), \
}, }

#define ROOK_PST { { \
   S(232, 311), S(188, 309), S(208, 305), S(157, 330), S(171, 327), S(178, 335), S(222, 311), S(108, 338), \
   S(282, 288), S(300, 287), S(254, 287), S(226, 301), S(244, 289), S(201, 322), S(139, 329), S(114, 333), \
   S(250, 295), S(273, 294), S(250, 287), S(264, 290), S(195, 308), S(175, 324), S(242, 300), S(111, 313), \
   S(184, 307), S(192, 313), S(187, 320), S(177, 321), S(190, 312), S(215, 325), S(97, 339), S(113, 326), \
   S(135, 308), S(132, 324), S(136, 326), S(176, 320), S(207, 300), S(162, 328), S(155, 308), S(149, 295), \
   S(161, 291), S(134, 312), S(184, 288), S(200, 273), S(136, 306), S(177, 304), S(120, 310), S(133, 287), \
   S(158, 288), S(152, 302), S(189, 281), S(172, 290), S(188, 293), S(182, 305), S(108, 307), S(126, 294), \
   S(180, 278), S(195, 287), S(175, 294), S(176, 294), S(188, 282), S(166, 302), S(119, 310), S(142, 290), \
}, { \
   S(297, 400), S(333, 393), S(274, 420), S(329, 393), S(312, 395), S(263, 419), S(304, 404), S(298, 397), \
   S(278, 410), S(280, 418), S(313, 411), S(340, 398), S(334, 386), S(352, 394), S(199, 444), S(259, 408), \
   S(256, 406), S(286, 410), S(280, 414), S(290, 409), S(304, 391), S(350, 382), S(327, 405), S(270, 395), \
   S(238, 410), S(282, 399), S(271, 417), S(276, 404), S(265, 399), S(268, 410), S(255, 402), S(243, 400), \
   S(245, 400), S(254, 404), S(264, 409), S(271, 400), S(290, 380), S(257, 399), S(271, 399), S(235, 396), \
   S(251, 381), S(263, 392), S(268, 387), S(260, 390), S(270, 387), S(287, 382), S(277, 388), S(249, 371), \
   S(238, 382), S(266, 375), S(265, 385), S(271, 388), S(281, 372), S(274, 382), S(271, 375), S(196, 387), \
   S(264, 380), S(271, 379), S(272, 384), S(276, 377), S(277, 372), S(267, 382), S(232, 390), S(255, 353), \
}, }

#define QUEEN_PST { { \
   S(467, 579), S(555, 511), S(629, 473), S(469, 504), S(664, 422), S(476, 545), S(548, 426), S(422, 560), \
   S(619, 447), S(505, 501), S(612, 446), S(530, 511), S(415, 553), S(429, 549), S(436, 513), S(435, 519), \
   S(551, 469), S(529, 516), S(591, 491), S(488, 526), S(476, 545), S(482, 534), S(427, 540), S(411, 551), \
   S(491, 517), S(466, 496), S(387, 557), S(457, 545), S(438, 506), S(450, 542), S(390, 559), S(455, 558), \
   S(480, 511), S(424, 555), S(418, 574), S(432, 554), S(411, 535), S(429, 598), S(408, 563), S(398, 568), \
   S(503, 454), S(461, 468), S(450, 515), S(459, 458), S(435, 524), S(487, 485), S(436, 542), S(437, 521), \
   S(428, 508), S(446, 445), S(482, 447), S(462, 477), S(479, 487), S(494, 497), S(407, 524), S(430, 450), \
   S(407, 498), S(436, 411), S(438, 469), S(472, 408), S(486, 433), S(434, 458), S(382, 511), S(372, 549), \
}, { \
   S(566, 699), S(575, 710), S(541, 767), S(621, 707), S(699, 660), S(599, 763), S(692, 689), S(637, 759), \
   S(557, 722), S(555, 716), S(582, 757), S(573, 776), S(521, 835), S(674, 714), S(640, 757), S(676, 746), \
   S(592, 695), S(590, 697), S(586, 716), S(587, 756), S(638, 717), S(679, 744), S(680, 692), S(639, 748), \
   S(560, 739), S(579, 721), S(582, 722), S(575, 751), S(581, 766), S(605, 754), S(592, 777), S(602, 745), \
   S(592, 684), S(569, 723), S(585, 728), S(575, 760), S(593, 725), S(577, 746), S(605, 730), S(596, 749), \
   S(583, 709), S(601, 672), S(591, 718), S(596, 692), S(595, 709), S(600, 716), S(610, 704), S(596, 728), \
   S(587, 672), S(597, 668), S(608, 662), S(615, 651), S(616, 656), S(607, 674), S(600, 626), S(600, 687), \
   S(602, 666), S(587, 654), S(596, 663), S(611, 628), S(598, 645), S(565, 679), S(566, 658), S(574, 637), \
}, }

#define PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(95, 218), S(35, 198), S(57, 197), S(155, 194), S(44, 274), S(57, 244), S(31, 283), S(69, 280), \
   S(97, 152), S(201, 110), S(250, 111), S(221, 139), S(51, 178), S(164, 133), S(102, 173), S(121, 171), \
   S(62, 137), S(69, 132), S(108, 125), S(117, 130), S(113, 136), S(114, 130), S(151, 126), S(156, 134), \
   S(42, 132), S(47, 139), S(94, 125), S(120, 122), S(122, 121), S(118, 121), S(122, 119), S(124, 124), \
   S(34, 127), S(52, 131), S(87, 126), S(79, 137), S(111, 125), S(119, 125), S(142, 101), S(141, 108), \
   S(37, 137), S(56, 134), S(93, 127), S(89, 132), S(102, 133), S(132, 122), S(145, 106), S(123, 114), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(112, 271), S(98, 268), S(85, 261), S(120, 217), S(93, 217), S(83, 188), S(29, 219), S(7, 252), \
   S(93, 183), S(92, 176), S(120, 162), S(133, 145), S(194, 138), S(186, 134), S(205, 145), S(160, 158), \
   S(86, 161), S(108, 141), S(106, 137), S(122, 110), S(120, 130), S(122, 123), S(97, 132), S(101, 135), \
   S(75, 146), S(95, 132), S(107, 127), S(131, 117), S(118, 125), S(120, 117), S(88, 120), S(92, 123), \
   S(79, 134), S(100, 121), S(105, 122), S(113, 130), S(119, 140), S(122, 124), S(128, 108), S(110, 114), \
   S(76, 139), S(107, 117), S(90, 139), S(106, 135), S(104, 157), S(137, 129), S(140, 109), S(102, 110), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define KING_PST { { \
   S(-70, -86), S(-4, -10), S(20, -63), S(-185, 2), S(40, 37), S(43, 51), S(280, 2), S(319, -45), \
   S(171, -72), S(54, 30), S(160, 15), S(86, 19), S(226, 48), S(205, 69), S(277, 38), S(266, -4), \
   S(-37, 27), S(18, 30), S(69, 52), S(22, 54), S(304, 59), S(346, 58), S(394, 38), S(240, 19), \
   S(-39, -11), S(-33, 40), S(13, 42), S(-119, 67), S(268, 63), S(250, 61), S(383, 35), S(251, 19), \
   S(-148, 1), S(-25, 4), S(12, 29), S(-81, 54), S(234, 56), S(300, 38), S(299, 15), S(209, -6), \
   S(-31, -33), S(19, -7), S(-11, 16), S(14, 22), S(275, 35), S(303, 14), S(299, -3), S(294, -17), \
   S(7, -40), S(46, -31), S(41, -10), S(-37, 14), S(298, 19), S(280, 1), S(309, -21), S(305, -50), \
   S(18, -90), S(49, -59), S(4, -38), S(-73, -7), S(277, -20), S(253, -18), S(307, -55), S(285, -95), \
}, { \
   S(-244, -116), S(-79, -93), S(-63, -55), S(-333, 8), S(-91, 18), S(0, -3), S(120, -17), S(-69, -40), \
   S(-302, 17), S(-208, 18), S(-333, 38), S(-289, 42), S(18, 50), S(1, 58), S(149, 10), S(-178, 47), \
   S(-350, 29), S(-215, 29), S(-170, 22), S(-340, 55), S(87, 47), S(251, 51), S(233, 45), S(16, 12), \
   S(-249, 6), S(-220, 28), S(-252, 39), S(-322, 61), S(-81, 76), S(-15, 62), S(-34, 46), S(-167, 25), \
   S(-419, 18), S(-224, -1), S(-269, 28), S(-387, 61), S(-87, 80), S(-35, 50), S(-54, 28), S(-106, 6), \
   S(-301, -22), S(-292, 1), S(-279, 13), S(-348, 40), S(10, 44), S(4, 29), S(10, 11), S(-28, -15), \
   S(-228, -68), S(-257, -24), S(-291, 4), S(-346, 24), S(-35, 37), S(-11, 24), S(23, -2), S(11, -32), \
   S(-273, -116), S(-232, -85), S(-252, -49), S(-390, 2), S(-19, -11), S(-55, 5), S(20, -37), S(7, -78), \
}, }

#endif