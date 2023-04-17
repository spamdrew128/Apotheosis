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
   S(39, 68)

#define PASSED_PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(35, 336), S(49, 277), S(30, 291), S(34, 307), S(56, 350), S(6, 366), S(-25, 376), S(32, 372), \
   S(-27, 186), S(-48, 182), S(-4, 121), S(-115, 119), S(-1, 124), S(-20, 221), S(-66, 262), S(-71, 305), \
   S(-6, 84), S(16, 70), S(32, 52), S(36, 41), S(-18, 79), S(-58, 111), S(-61, 149), S(-115, 187), \
   S(6, 49), S(-3, 44), S(6, 25), S(0, 17), S(-49, 41), S(-65, 56), S(-15, 77), S(-60, 94), \
   S(54, 9), S(2, 24), S(41, 4), S(-4, -1), S(-42, 15), S(-27, 20), S(-51, 41), S(-7, 30), \
   S(6, 4), S(69, 18), S(86, 7), S(-15, -6), S(-32, 8), S(-28, 9), S(-38, 30), S(-30, 27), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(86, 365), S(89, 346), S(37, 358), S(83, 316), S(101, 298), S(38, 285), S(43, 305), S(-8, 342), \
   S(49, 245), S(11, 263), S(5, 208), S(35, 151), S(8, 96), S(63, 109), S(-10, 111), S(-17, 163), \
   S(16, 134), S(7, 129), S(15, 95), S(8, 81), S(17, 57), S(62, 43), S(19, 69), S(27, 69), \
   S(12, 71), S(-19, 74), S(-31, 60), S(-10, 37), S(-12, 36), S(-17, 39), S(14, 51), S(42, 38), \
   S(3, 26), S(-7, 33), S(-33, 30), S(-30, 21), S(5, 6), S(11, 11), S(25, 34), S(47, 9), \
   S(0, 21), S(2, 26), S(2, 8), S(-28, 17), S(25, 5), S(22, 6), S(34, 33), S(15, 21), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define BLOCKED_PASSERS { \
   S(124, -425), S(19, -132), S(-4, -50), S(-6, -18), S(-1, 0), S(-18, 0), S(0, 0), S(0, 0), \
}

#define ROOK_OPEN_FILE { \
   S(47, -4), S(59, -9), S(52, -8), S(54, -6), S(61, -7), S(64, -15), S(125, -26), S(124, -21), \
}

#define ROOK_SEMI_OPEN_FILE { \
   S(-8, 52), S(15, 21), S(17, 10), S(27, 2), S(28, -8), S(21, -11), S(52, -12), S(23, 31), \
}

#define QUEEN_OPEN_FILE { \
   S(0, 3), S(-3, 19), S(-20, 21), S(-22, 37), S(-7, 35), S(-5, 5), S(30, -26), S(71, -40), \
}

#define QUEEN_SEMI_OPEN_FILE { \
   S(1, 0), S(15, 3), S(2, 7), S(-10, 52), S(10, 20), S(22, -39), S(46, -66), S(40, -38), \
}

#define KING_OPEN_FILE { \
   S(-78, -15), S(-111, -1), S(-88, -3), S(-42, -11), S(-65, -3), S(-84, 7), S(-108, 15), S(-55, 2), \
}

#define KING_SEMI_OPEN_FILE { \
   S(-22, 35), S(-51, 17), S(-23, -3), S(-14, -6), S(-21, -2), S(-27, 1), S(-40, 17), S(-44, 35), \
}

#define ISOLATED_PAWNS { \
   S(-18, -19), S(-23, -14), S(-23, -17), S(-37, -11), S(-33, -19), S(-8, -17), S(-17, -17), S(-33, -14), \
}

#define KNIGHT_MOBILITY { \
   S(286, 138), S(308, 198), S(321, 227), S(325, 243), S(337, 246), S(345, 257), S(352, 256), S(359, 253), S(382, 237), \
}

#define BISHOP_MOBILITY { \
   S(273, 157), S(289, 190), S(305, 200), S(315, 215), S(324, 232), S(327, 244), S(331, 250), S(335, 252), S(334, 260), S(343, 256), S(354, 256), S(378, 248), S(347, 283), S(410, 240), \
}

#define ROOK_MOBILITY { \
   S(352, 309), S(366, 338), S(370, 350), S(375, 364), S(376, 376), S(382, 384), S(384, 391), S(390, 395), S(400, 400), S(405, 405), S(404, 411), S(412, 417), S(412, 424), S(421, 419), S(427, 415), \
}

#define QUEEN_MOBILITY { \
   S(842, 485), S(839, 562), S(837, 516), S(837, 538), S(842, 542), S(842, 570), S(842, 601), S(843, 636), S(846, 648), S(849, 663), S(848, 675), S(852, 687), S(850, 696), S(850, 710), S(856, 720), S(852, 733), S(849, 743), S(848, 750), S(874, 740), S(880, 744), S(909, 738), S(917, 747), S(950, 724), S(962, 730), S(972, 715), S(1157, 626), S(862, 822), S(773, 826), \
}

/*
Average PST values for MG, EG:
Knight Values: 164 116
Bishop Values: 181 153
Rook Values: 245 326
Queen Values: 601 566
Pawn Values: 111 157
*/

#define KNIGHT_PST { { \
   S(-73, 7), S(219, -3), S(-9, 65), S(161, 55), S(250, 42), S(163, 53), S(109, 57), S(72, 18), \
   S(138, 44), S(216, 55), S(163, 74), S(244, 66), S(189, 89), S(264, 58), S(138, 84), S(31, 98), \
   S(171, 55), S(229, 60), S(219, 89), S(214, 98), S(215, 96), S(210, 87), S(187, 93), S(76, 85), \
   S(161, 96), S(174, 92), S(221, 118), S(203, 113), S(220, 118), S(216, 103), S(188, 92), S(120, 100), \
   S(165, 78), S(214, 85), S(186, 126), S(200, 114), S(164, 129), S(183, 112), S(146, 97), S(103, 121), \
   S(136, 70), S(151, 101), S(154, 107), S(165, 120), S(195, 105), S(172, 94), S(148, 97), S(106, 111), \
   S(68, 73), S(144, 96), S(129, 101), S(150, 114), S(148, 105), S(138, 103), S(109, 102), S(87, 98), \
   S(99, 45), S(122, 81), S(76, 113), S(144, 92), S(102, 96), S(135, 77), S(133, 52), S(35, 96), \
}, { \
   S(-40, 137), S(-26, 173), S(78, 146), S(95, 145), S(186, 128), S(38, 149), S(116, 82), S(44, 40), \
   S(87, 141), S(112, 172), S(210, 139), S(221, 146), S(196, 156), S(248, 111), S(165, 124), S(150, 116), \
   S(112, 152), S(195, 140), S(203, 178), S(240, 166), S(282, 148), S(358, 123), S(255, 132), S(193, 117), \
   S(172, 150), S(195, 164), S(200, 184), S(249, 183), S(215, 188), S(263, 181), S(192, 180), S(232, 143), \
   S(168, 153), S(176, 168), S(194, 183), S(204, 186), S(211, 190), S(211, 183), S(222, 159), S(186, 152), \
   S(158, 142), S(173, 152), S(193, 153), S(191, 174), S(217, 174), S(204, 150), S(208, 149), S(166, 152), \
   S(153, 109), S(166, 123), S(173, 149), S(206, 140), S(206, 145), S(198, 137), S(199, 138), S(178, 130), \
   S(64, 142), S(172, 121), S(152, 132), S(166, 150), S(190, 149), S(179, 144), S(174, 136), S(151, 113), \
}, }

#define BISHOP_PST { { \
   S(29, 141), S(147, 120), S(104, 109), S(66, 107), S(-35, 145), S(41, 131), S(78, 124), S(118, 125), \
   S(178, 93), S(176, 112), S(137, 118), S(135, 115), S(210, 115), S(117, 134), S(123, 146), S(22, 152), \
   S(202, 114), S(325, 82), S(185, 127), S(221, 118), S(200, 131), S(223, 118), S(122, 144), S(120, 137), \
   S(185, 116), S(172, 127), S(214, 129), S(229, 119), S(201, 148), S(191, 134), S(207, 122), S(133, 131), \
   S(163, 118), S(159, 129), S(149, 142), S(195, 138), S(210, 128), S(222, 121), S(177, 119), S(143, 127), \
   S(148, 122), S(162, 130), S(171, 138), S(171, 134), S(211, 131), S(206, 124), S(225, 129), S(182, 137), \
   S(184, 105), S(173, 133), S(166, 115), S(182, 122), S(185, 121), S(214, 119), S(223, 112), S(162, 119), \
   S(59, 138), S(122, 142), S(176, 127), S(141, 140), S(179, 122), S(163, 126), S(153, 126), S(114, 138), \
}, { \
   S(177, 176), S(132, 183), S(100, 190), S(85, 187), S(110, 184), S(136, 178), S(151, 177), S(182, 156), \
   S(136, 201), S(192, 185), S(185, 180), S(156, 179), S(187, 180), S(209, 165), S(198, 176), S(136, 166), \
   S(154, 194), S(200, 192), S(212, 191), S(212, 185), S(231, 180), S(267, 191), S(231, 180), S(198, 179), \
   S(179, 189), S(207, 190), S(205, 201), S(262, 191), S(234, 203), S(222, 199), S(210, 184), S(185, 184), \
   S(199, 175), S(206, 187), S(215, 199), S(240, 202), S(236, 194), S(213, 192), S(214, 175), S(212, 168), \
   S(214, 174), S(228, 187), S(223, 194), S(225, 192), S(220, 200), S(235, 185), S(218, 181), S(211, 176), \
   S(233, 159), S(237, 155), S(228, 167), S(216, 177), S(227, 177), S(240, 176), S(258, 164), S(223, 154), \
   S(198, 162), S(225, 170), S(210, 178), S(194, 183), S(211, 181), S(205, 183), S(220, 167), S(205, 151), \
}, }

#define ROOK_PST { { \
   S(213, 288), S(174, 299), S(172, 299), S(183, 300), S(195, 299), S(221, 295), S(192, 291), S(219, 276), \
   S(274, 262), S(272, 266), S(261, 263), S(263, 264), S(219, 276), S(241, 282), S(158, 292), S(156, 290), \
   S(260, 268), S(270, 264), S(294, 268), S(279, 253), S(241, 266), S(198, 294), S(195, 278), S(145, 277), \
   S(213, 277), S(187, 283), S(170, 290), S(225, 283), S(194, 292), S(244, 286), S(124, 297), S(171, 276), \
   S(144, 286), S(174, 284), S(170, 287), S(172, 285), S(187, 283), S(204, 287), S(134, 288), S(135, 285), \
   S(144, 276), S(175, 273), S(199, 269), S(205, 259), S(171, 274), S(214, 275), S(146, 285), S(170, 256), \
   S(174, 252), S(156, 274), S(189, 258), S(196, 264), S(173, 275), S(182, 286), S(135, 267), S(145, 262), \
   S(202, 257), S(214, 261), S(204, 265), S(210, 273), S(216, 268), S(216, 279), S(146, 294), S(188, 264), \
}, { \
   S(322, 369), S(308, 382), S(263, 400), S(314, 379), S(305, 387), S(280, 399), S(330, 385), S(350, 368), \
   S(296, 385), S(282, 399), S(318, 390), S(359, 374), S(354, 371), S(342, 380), S(231, 411), S(289, 381), \
   S(272, 382), S(297, 387), S(297, 390), S(330, 375), S(325, 371), S(358, 368), S(336, 372), S(313, 366), \
   S(258, 386), S(280, 383), S(290, 397), S(294, 381), S(285, 384), S(310, 383), S(295, 384), S(274, 376), \
   S(265, 372), S(247, 391), S(272, 391), S(269, 384), S(279, 370), S(277, 378), S(275, 380), S(278, 355), \
   S(267, 361), S(266, 375), S(277, 376), S(279, 372), S(297, 363), S(303, 364), S(301, 359), S(276, 349), \
   S(267, 360), S(277, 359), S(278, 374), S(290, 367), S(302, 356), S(311, 358), S(320, 346), S(232, 354), \
   S(296, 364), S(292, 363), S(297, 374), S(306, 366), S(309, 361), S(316, 364), S(269, 372), S(309, 324), \
}, }

#define QUEEN_PST { { \
   S(541, 471), S(582, 457), S(731, 381), S(491, 464), S(577, 457), S(495, 503), S(642, 393), S(454, 476), \
   S(695, 383), S(593, 423), S(616, 448), S(496, 532), S(580, 469), S(502, 485), S(429, 519), S(505, 443), \
   S(603, 410), S(646, 426), S(696, 445), S(608, 434), S(519, 495), S(505, 494), S(490, 496), S(453, 487), \
   S(540, 490), S(538, 458), S(504, 478), S(496, 498), S(515, 448), S(515, 461), S(479, 493), S(511, 454), \
   S(538, 481), S(487, 502), S(491, 549), S(537, 461), S(508, 430), S(496, 522), S(486, 500), S(460, 475), \
   S(543, 446), S(528, 442), S(518, 450), S(521, 418), S(534, 448), S(570, 423), S(511, 501), S(494, 434), \
   S(527, 422), S(513, 433), S(564, 386), S(534, 438), S(563, 400), S(589, 413), S(469, 497), S(498, 420), \
   S(475, 467), S(522, 419), S(531, 420), S(557, 383), S(549, 387), S(530, 448), S(484, 470), S(475, 442), \
}, { \
   S(627, 661), S(626, 686), S(629, 695), S(657, 692), S(704, 673), S(730, 694), S(771, 689), S(691, 734), \
   S(634, 668), S(614, 672), S(629, 711), S(626, 718), S(585, 791), S(667, 751), S(656, 758), S(685, 752), \
   S(642, 657), S(665, 642), S(655, 677), S(664, 701), S(688, 699), S(717, 744), S(700, 723), S(689, 757), \
   S(630, 681), S(648, 673), S(646, 686), S(650, 687), S(635, 744), S(641, 775), S(643, 785), S(655, 738), \
   S(674, 639), S(640, 681), S(671, 668), S(663, 687), S(657, 693), S(654, 712), S(658, 734), S(663, 735), \
   S(666, 637), S(692, 613), S(674, 664), S(686, 642), S(687, 646), S(679, 677), S(683, 692), S(669, 692), \
   S(668, 622), S(690, 616), S(707, 605), S(710, 609), S(712, 600), S(707, 613), S(702, 622), S(672, 671), \
   S(678, 620), S(669, 622), S(691, 606), S(713, 578), S(696, 596), S(660, 640), S(647, 663), S(669, 604), \
}, }

#define PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(35, 336), S(49, 277), S(30, 291), S(34, 307), S(56, 350), S(6, 366), S(-25, 376), S(32, 372), \
   S(134, 139), S(236, 92), S(215, 128), S(297, 132), S(141, 179), S(142, 132), S(94, 174), S(120, 151), \
   S(67, 134), S(109, 110), S(134, 110), S(150, 106), S(141, 119), S(135, 120), S(121, 133), S(128, 130), \
   S(52, 127), S(68, 116), S(120, 107), S(142, 107), S(132, 114), S(138, 109), S(103, 119), S(119, 116), \
   S(43, 121), S(67, 112), S(103, 114), S(105, 118), S(131, 115), S(131, 110), S(132, 102), S(128, 104), \
   S(48, 130), S(77, 114), S(112, 115), S(125, 111), S(129, 118), S(164, 107), S(148, 102), S(117, 108), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(86, 365), S(89, 346), S(37, 358), S(83, 316), S(101, 298), S(38, 285), S(43, 305), S(-8, 342), \
   S(86, 177), S(119, 150), S(141, 143), S(152, 141), S(214, 153), S(212, 126), S(197, 153), S(139, 150), \
   S(87, 154), S(114, 132), S(114, 129), S(142, 103), S(143, 112), S(139, 109), S(111, 123), S(107, 121), \
   S(72, 140), S(99, 123), S(118, 114), S(142, 108), S(135, 113), S(142, 102), S(101, 112), S(100, 111), \
   S(81, 125), S(102, 110), S(113, 114), S(119, 117), S(127, 127), S(132, 110), S(135, 101), S(115, 103), \
   S(77, 132), S(116, 108), S(96, 131), S(120, 128), S(112, 140), S(161, 114), S(152, 103), S(114, 100), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define KING_PST { { \
   S(43, -99), S(112, -32), S(76, -79), S(217, -84), S(256, -33), S(342, -13), S(327, -42), S(247, -57), \
   S(69, -41), S(216, 20), S(164, 9), S(25, 14), S(273, 37), S(341, 29), S(359, 14), S(397, -47), \
   S(89, -14), S(133, 10), S(291, 22), S(-3, 36), S(301, 43), S(396, 32), S(384, 13), S(324, -1), \
   S(-68, -21), S(51, 17), S(30, 31), S(-94, 61), S(248, 60), S(226, 61), S(313, 42), S(286, 10), \
   S(-111, -9), S(-77, 15), S(-92, 45), S(-163, 66), S(178, 73), S(276, 44), S(295, 16), S(228, -2), \
   S(-127, -20), S(-29, -1), S(-114, 38), S(-143, 51), S(241, 50), S(229, 37), S(298, 4), S(307, -23), \
   S(8, -44), S(-3, -14), S(-42, 6), S(-130, 28), S(248, 33), S(277, 11), S(319, -15), S(318, -39), \
   S(0, -93), S(44, -71), S(-27, -36), S(-129, -3), S(286, -20), S(244, -14), S(346, -61), S(321, -87), \
}, { \
   S(-120, -151), S(-190, -62), S(-163, -54), S(-250, -31), S(-33, -19), S(38, -19), S(322, -89), S(100, -62), \
   S(-115, -60), S(-203, 3), S(-181, 0), S(-308, 25), S(61, 17), S(93, 17), S(126, 20), S(-66, 21), \
   S(-254, -4), S(-192, 13), S(-244, 21), S(-281, 42), S(59, 39), S(267, 30), S(187, 44), S(23, 9), \
   S(-280, -4), S(-307, 34), S(-316, 41), S(-359, 62), S(-38, 57), S(21, 47), S(-76, 48), S(-204, 23), \
   S(-368, -6), S(-257, -1), S(-346, 40), S(-430, 62), S(-95, 71), S(-101, 56), S(-90, 29), S(-153, 4), \
   S(-315, -27), S(-314, 0), S(-361, 28), S(-427, 53), S(-95, 62), S(-67, 42), S(-29, 13), S(-57, -14), \
   S(-222, -74), S(-280, -23), S(-358, 14), S(-390, 28), S(-92, 46), S(-51, 28), S(14, -3), S(18, -34), \
   S(-256, -110), S(-211, -91), S(-246, -56), S(-393, -5), S(-19, -15), S(-65, -3), S(40, -48), S(24, -89), \
}, }

#define SAFETY_TABLE { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 1), S(3, 5), S(5, -15), S(6, 21), \
   S(6, 4), S(10, 18), S(10, 8), S(12, 5), S(10, 15), S(25, 4), S(15, 11), S(11, 17), S(7, 15), S(18, 22), \
   S(32, -7), S(13, 67), S(23, 7), S(28, 29), S(18, 44), S(49, 39), S(20, 63), S(30, 35), S(45, 35), S(28, 53), \
   S(66, 43), S(45, 64), S(59, 25), S(66, 50), S(48, 82), S(79, 98), S(75, 58), S(107, -12), S(66, 63), S(105, 17), \
   S(86, 186), S(123, 108), S(108, 80), S(163, 34), S(99, 82), S(178, 39), S(205, 59), S(194, -6), S(254, -91), S(233, -17), \
   S(416, -121), S(249, 30), S(187, 62), S(235, -17), S(211, 64), S(243, 103), S(401, -112), S(291, 164), S(221, 212), S(461, -142), \
   S(139, 184), S(279, 193), S(309, 92), S(440, -89), S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), \
   S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), S(402, 280), \
}

#endif