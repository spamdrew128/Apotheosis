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
   S(75, 209), S(30, 187), S(66, 186), S(158, 182), S(47, 260), S(50, 231), S(17, 272), S(53, 269), \
   S(46, 167), S(-60, 192), S(-39, 143), S(-31, 94), S(52, 139), S(-78, 233), S(-67, 271), S(-67, 288), \
   S(2, 87), S(0, 69), S(94, 35), S(3, 45), S(4, 68), S(-53, 111), S(-118, 161), S(-113, 185), \
   S(-7, 53), S(-31, 47), S(39, 23), S(-1, 22), S(-73, 54), S(-68, 62), S(-46, 87), S(-56, 89), \
   S(87, 8), S(-18, 30), S(-8, 13), S(3, 17), S(-57, 24), S(-13, 7), S(-31, 34), S(-31, 43), \
   S(13, 9), S(95, 19), S(106, -3), S(-15, 9), S(-20, 17), S(-44, 20), S(-40, 23), S(-42, 30), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(99, 259), S(88, 255), S(75, 247), S(114, 204), S(80, 207), S(128, 163), S(18, 209), S(0, 239), \
   S(50, 230), S(16, 239), S(22, 205), S(23, 159), S(-11, 137), S(58, 108), S(-50, 128), S(-46, 155), \
   S(27, 129), S(-5, 127), S(8, 99), S(11, 85), S(13, 55), S(71, 37), S(6, 79), S(18, 72), \
   S(5, 72), S(-24, 74), S(-39, 60), S(-19, 46), S(-25, 41), S(4, 33), S(16, 50), S(34, 45), \
   S(1, 21), S(-15, 39), S(-31, 31), S(-30, 26), S(6, 7), S(42, 5), S(47, 30), S(62, 8), \
   S(-5, 19), S(7, 26), S(7, 14), S(-23, 26), S(6, 8), S(19, 8), S(70, 23), S(10, 27), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define BLOCKED_PASSERS { \
   S(69, -208), S(12, -122), S(-7, -46), S(2, -25), S(-17, 5), S(3, -16), S(0, 0), S(0, 0), \
}

#define ROOK_OPEN_FILE { \
   S(28, 1), S(34, 1), S(35, 1), S(41, 0), S(42, 5), S(61, -11), S(119, -24), S(119, -17), \
}

#define ROOK_SEMI_OPEN_FILE { \
   S(-13, 44), S(4, 13), S(13, 6), S(25, -5), S(20, -8), S(19, -10), S(44, -14), S(25, 20), \
}

#define QUEEN_OPEN_FILE { \
   S(-16, 11), S(-9, 27), S(-20, 22), S(-20, 32), S(-11, 36), S(7, 8), S(34, 9), S(46, -22), \
}

#define QUEEN_SEMI_OPEN_FILE { \
   S(-9, 3), S(5, 1), S(4, 0), S(-4, 28), S(-1, 29), S(19, -41), S(52, -55), S(29, -19), \
}

#define KING_OPEN_FILE { \
   S(-76, -15), S(-130, 6), S(-100, 2), S(-45, -12), S(-70, -4), S(-102, 8), S(-108, 18), S(-71, 8), \
}

#define KING_SEMI_OPEN_FILE { \
   S(-50, 41), S(-73, 22), S(-24, -3), S(-1, -9), S(-19, 1), S(-32, 1), S(-45, 20), S(-47, 36), \
}

#define ISOLATED_PAWNS { \
   S(-15, -17), S(-18, -16), S(-18, -22), S(-32, -16), S(-30, -23), S(-10, -19), S(-16, -16), S(-28, -19), \
}

#define KNIGHT_MOBILITY { \
   S(242, 167), S(261, 233), S(273, 256), S(278, 267), S(292, 272), S(299, 283), S(310, 280), S(314, 280), S(337, 257), \
}

#define BISHOP_MOBILITY { \
   S(231, 186), S(246, 207), S(261, 227), S(270, 245), S(279, 259), S(284, 269), S(291, 277), S(296, 278), S(294, 285), S(304, 281), S(319, 276), S(348, 271), S(285, 308), S(359, 262), \
}

#define ROOK_MOBILITY { \
   S(289, 342), S(303, 369), S(309, 387), S(313, 400), S(313, 413), S(320, 420), S(323, 426), S(331, 429), S(342, 433), S(349, 438), S(352, 443), S(361, 446), S(360, 453), S(376, 447), S(376, 446), \
}

#define QUEEN_MOBILITY { \
   S(707, 610), S(701, 606), S(700, 603), S(699, 634), S(704, 673), S(707, 690), S(710, 698), S(709, 738), S(712, 749), S(717, 758), S(722, 770), S(724, 785), S(721, 795), S(731, 800), S(733, 814), S(732, 827), S(730, 838), S(746, 826), S(762, 828), S(770, 827), S(760, 846), S(820, 813), S(793, 831), S(882, 787), S(924, 758), S(936, 759), S(756, 895), S(707, 884), \
}

#define PAWN_THREAT_ON_KNIGHT S(61, 9)
#define PAWN_THREAT_ON_BISHOP S(64, 41)
#define PAWN_THREAT_ON_ROOK S(117, -38)
#define PAWN_THREAT_ON_QUEEN S(66, -11)

#define KNIGHT_THREAT_ON_BISHOP S(27, 24)
#define KNIGHT_THREAT_ON_ROOK S(64, -11)
#define KNIGHT_THREAT_ON_QUEEN S(27, -36)

#define BISHOP_THREAT_ON_KNIGHT S(23, 29)
#define BISHOP_THREAT_ON_ROOK S(44, 4)
#define BISHOP_THREAT_ON_QUEEN S(45, 33)

#define ROOK_THREAT_ON_QUEEN S(60, -21)

/*
Average PST values for MG, EG:
Knight Values: 152 130
Bishop Values: 170 165
Rook Values: 232 348
Queen Values: 545 620
Pawn Values: 104 142
*/

#define KNIGHT_PST { { \
   S(-72, 20), S(85, 26), S(20, 130), S(162, 67), S(194, 85), S(169, 63), S(115, 52), S(74, 58), \
   S(139, 67), S(213, 68), S(223, 82), S(192, 89), S(118, 111), S(165, 106), S(133, 103), S(26, 117), \
   S(96, 100), S(225, 85), S(208, 119), S(227, 123), S(170, 132), S(155, 116), S(101, 123), S(125, 91), \
   S(174, 95), S(184, 112), S(180, 146), S(200, 150), S(212, 132), S(188, 132), S(169, 116), S(87, 110), \
   S(155, 113), S(196, 100), S(172, 147), S(184, 141), S(162, 145), S(155, 135), S(153, 119), S(118, 123), \
   S(135, 90), S(149, 116), S(147, 122), S(160, 147), S(194, 126), S(154, 113), S(157, 90), S(113, 100), \
   S(85, 96), S(175, 109), S(115, 117), S(133, 127), S(135, 119), S(142, 102), S(105, 100), S(73, 115), \
   S(21, 96), S(118, 95), S(102, 124), S(110, 121), S(109, 104), S(129, 92), S(112, 96), S(94, 110), \
}, { \
   S(-20, 130), S(26, 142), S(67, 168), S(126, 138), S(185, 145), S(11, 161), S(68, 111), S(42, 5), \
   S(46, 164), S(93, 183), S(196, 138), S(173, 172), S(199, 154), S(226, 130), S(191, 127), S(178, 100), \
   S(104, 148), S(185, 153), S(191, 186), S(219, 178), S(240, 169), S(334, 156), S(231, 136), S(210, 127), \
   S(159, 163), S(176, 178), S(174, 202), S(231, 197), S(201, 205), S(241, 194), S(191, 186), S(213, 155), \
   S(152, 165), S(161, 164), S(180, 192), S(193, 198), S(201, 198), S(194, 196), S(221, 168), S(174, 163), \
   S(144, 147), S(152, 164), S(177, 161), S(179, 178), S(199, 186), S(188, 159), S(193, 151), S(158, 166), \
   S(134, 127), S(138, 147), S(157, 149), S(184, 150), S(184, 164), S(183, 148), S(188, 135), S(172, 135), \
   S(51, 149), S(154, 126), S(149, 136), S(153, 155), S(175, 152), S(171, 148), S(160, 139), S(164, 94), \
}, }

#define BISHOP_PST { { \
   S(168, 143), S(137, 130), S(28, 153), S(-59, 144), S(-37, 168), S(11, 144), S(56, 163), S(109, 145), \
   S(176, 117), S(180, 124), S(184, 123), S(170, 87), S(213, 130), S(117, 160), S(78, 164), S(21, 170), \
   S(236, 113), S(290, 106), S(213, 155), S(235, 133), S(177, 145), S(201, 122), S(134, 160), S(164, 149), \
   S(179, 119), S(157, 148), S(173, 153), S(234, 148), S(175, 162), S(185, 148), S(200, 132), S(115, 158), \
   S(152, 130), S(145, 145), S(134, 154), S(180, 151), S(209, 141), S(194, 136), S(142, 144), S(135, 156), \
   S(142, 146), S(149, 140), S(161, 147), S(161, 153), S(193, 149), S(203, 138), S(196, 156), S(163, 151), \
   S(164, 119), S(155, 142), S(189, 126), S(162, 143), S(175, 135), S(211, 117), S(220, 113), S(137, 160), \
   S(60, 150), S(79, 177), S(161, 151), S(160, 152), S(195, 134), S(168, 136), S(180, 134), S(158, 129), \
}, { \
   S(156, 190), S(131, 184), S(59, 203), S(39, 211), S(122, 187), S(70, 200), S(219, 172), S(171, 164), \
   S(138, 200), S(151, 202), S(134, 192), S(108, 207), S(180, 184), S(223, 174), S(179, 186), S(135, 174), \
   S(162, 194), S(194, 191), S(204, 185), S(197, 184), S(231, 182), S(254, 199), S(243, 186), S(221, 186), \
   S(158, 193), S(189, 192), S(188, 207), S(234, 207), S(205, 212), S(217, 202), S(185, 190), S(185, 197), \
   S(170, 190), S(184, 196), S(199, 202), S(218, 212), S(220, 199), S(188, 198), S(187, 193), S(187, 178), \
   S(190, 180), S(207, 192), S(202, 201), S(206, 197), S(209, 206), S(214, 193), S(202, 183), S(198, 181), \
   S(201, 178), S(212, 164), S(213, 176), S(200, 186), S(207, 186), S(223, 178), S(233, 171), S(207, 164), \
   S(182, 170), S(222, 176), S(195, 192), S(183, 187), S(204, 184), S(187, 197), S(205, 181), S(200, 165), \
}, }

#define ROOK_PST { { \
   S(250, 311), S(176, 317), S(281, 293), S(167, 335), S(174, 336), S(153, 348), S(188, 329), S(100, 345), \
   S(304, 283), S(345, 279), S(284, 285), S(232, 305), S(241, 296), S(171, 332), S(125, 334), S(111, 337), \
   S(274, 290), S(299, 289), S(296, 277), S(288, 287), S(199, 311), S(163, 328), S(230, 302), S(92, 320), \
   S(212, 299), S(224, 304), S(217, 315), S(202, 315), S(210, 307), S(222, 321), S(107, 334), S(117, 325), \
   S(142, 308), S(159, 320), S(155, 323), S(180, 320), S(204, 304), S(160, 331), S(157, 309), S(148, 299), \
   S(164, 294), S(162, 306), S(206, 285), S(205, 274), S(145, 311), S(174, 311), S(138, 310), S(135, 292), \
   S(167, 288), S(170, 302), S(209, 280), S(182, 294), S(186, 299), S(181, 311), S(116, 312), S(132, 299), \
   S(191, 283), S(211, 287), S(196, 295), S(193, 298), S(200, 288), S(188, 305), S(129, 317), S(156, 296), \
}, { \
   S(280, 399), S(309, 393), S(247, 423), S(306, 397), S(299, 396), S(273, 413), S(300, 400), S(301, 393), \
   S(260, 406), S(260, 413), S(295, 407), S(319, 397), S(320, 384), S(373, 382), S(232, 429), S(269, 398), \
   S(240, 401), S(274, 403), S(265, 407), S(282, 402), S(299, 381), S(375, 368), S(346, 393), S(296, 380), \
   S(234, 400), S(276, 388), S(264, 406), S(271, 393), S(275, 383), S(288, 393), S(279, 386), S(263, 384), \
   S(236, 394), S(241, 398), S(250, 404), S(258, 395), S(283, 373), S(263, 387), S(283, 388), S(247, 381), \
   S(243, 380), S(252, 389), S(254, 385), S(246, 390), S(263, 383), S(293, 373), S(296, 375), S(261, 360), \
   S(233, 381), S(255, 376), S(255, 383), S(262, 388), S(274, 371), S(288, 373), S(292, 365), S(211, 377), \
   S(263, 380), S(265, 380), S(266, 384), S(277, 377), S(282, 372), S(287, 378), S(247, 382), S(272, 342), \
}, }

#define QUEEN_PST { { \
   S(496, 586), S(542, 542), S(694, 478), S(544, 511), S(643, 471), S(532, 535), S(504, 483), S(432, 563), \
   S(653, 469), S(566, 516), S(688, 453), S(528, 552), S(448, 564), S(443, 556), S(424, 535), S(426, 538), \
   S(622, 478), S(584, 539), S(669, 522), S(557, 538), S(500, 565), S(477, 547), S(429, 535), S(423, 544), \
   S(531, 558), S(511, 552), S(420, 601), S(492, 574), S(469, 521), S(474, 544), S(396, 567), S(466, 551), \
   S(509, 553), S(463, 597), S(453, 617), S(459, 580), S(439, 539), S(447, 604), S(430, 560), S(410, 573), \
   S(532, 495), S(487, 492), S(478, 547), S(480, 488), S(466, 531), S(508, 492), S(456, 542), S(437, 542), \
   S(447, 530), S(464, 472), S(504, 475), S(485, 503), S(502, 502), S(529, 484), S(429, 533), S(451, 469), \
   S(427, 513), S(448, 465), S(452, 508), S(494, 447), S(505, 463), S(463, 471), S(396, 528), S(396, 545), \
}, { \
   S(552, 697), S(562, 716), S(551, 759), S(609, 721), S(704, 655), S(630, 751), S(715, 691), S(644, 768), \
   S(543, 705), S(526, 716), S(563, 754), S(550, 783), S(511, 834), S(688, 721), S(628, 798), S(672, 757), \
   S(583, 674), S(577, 672), S(582, 694), S(574, 759), S(648, 725), S(690, 776), S(693, 735), S(651, 786), \
   S(551, 715), S(573, 696), S(577, 707), S(569, 742), S(581, 771), S(604, 790), S(590, 830), S(604, 782), \
   S(588, 666), S(561, 708), S(580, 713), S(574, 745), S(588, 726), S(578, 767), S(601, 767), S(596, 784), \
   S(580, 691), S(598, 661), S(589, 703), S(598, 684), S(597, 702), S(605, 723), S(609, 728), S(597, 748), \
   S(585, 657), S(595, 658), S(611, 652), S(620, 640), S(621, 646), S(616, 676), S(605, 643), S(606, 685), \
   S(601, 655), S(586, 652), S(599, 656), S(619, 625), S(601, 646), S(573, 685), S(574, 664), S(576, 629), \
}, }

#define PAWN_PST { { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(75, 209), S(30, 187), S(66, 186), S(158, 182), S(47, 260), S(50, 231), S(17, 272), S(53, 269), \
   S(88, 148), S(208, 99), S(233, 109), S(212, 138), S(58, 169), S(170, 125), S(88, 171), S(113, 163), \
   S(55, 133), S(66, 122), S(112, 118), S(120, 121), S(111, 130), S(116, 124), S(149, 122), S(152, 131), \
   S(32, 128), S(45, 128), S(96, 117), S(121, 113), S(118, 114), S(120, 112), S(116, 115), S(119, 120), \
   S(22, 125), S(48, 121), S(83, 120), S(75, 128), S(111, 117), S(122, 117), S(133, 98), S(138, 103), \
   S(28, 134), S(52, 127), S(93, 121), S(92, 123), S(104, 125), S(147, 112), S(143, 101), S(127, 108), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, { \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
   S(99, 259), S(88, 255), S(75, 247), S(114, 204), S(80, 207), S(128, 163), S(18, 209), S(0, 239), \
   S(81, 182), S(83, 169), S(108, 157), S(134, 136), S(191, 122), S(190, 124), S(205, 136), S(163, 149), \
   S(76, 158), S(97, 136), S(102, 130), S(119, 101), S(123, 122), S(127, 112), S(99, 122), S(101, 128), \
   S(67, 143), S(87, 127), S(102, 121), S(129, 109), S(122, 116), S(122, 107), S(93, 110), S(92, 115), \
   S(71, 130), S(91, 115), S(101, 116), S(109, 123), S(115, 133), S(119, 116), S(122, 102), S(105, 109), \
   S(72, 136), S(100, 113), S(89, 133), S(109, 127), S(106, 149), S(146, 120), S(139, 103), S(105, 103), \
   S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), S(0, 0), \
}, }

#define KING_PST { { \
   S(-29, -95), S(72, -26), S(56, -65), S(-138, -6), S(41, 26), S(17, 55), S(292, -6), S(275, -43), \
   S(181, -83), S(157, 11), S(197, 0), S(134, 4), S(227, 35), S(224, 52), S(266, 26), S(311, -20), \
   S(2, 12), S(30, 22), S(58, 44), S(36, 43), S(245, 59), S(309, 54), S(374, 28), S(249, 8), \
   S(-86, -9), S(-8, 33), S(-28, 46), S(-147, 69), S(205, 66), S(184, 66), S(330, 35), S(263, 11), \
   S(-160, 2), S(-42, 3), S(-51, 43), S(-164, 68), S(128, 71), S(194, 53), S(223, 22), S(159, 0), \
   S(-28, -33), S(-17, -2), S(-99, 33), S(-63, 38), S(163, 54), S(195, 32), S(228, 7), S(254, -15), \
   S(36, -46), S(46, -31), S(3, -4), S(-77, 21), S(230, 27), S(223, 8), S(271, -19), S(298, -50), \
   S(74, -101), S(94, -71), S(39, -50), S(-53, -10), S(277, -27), S(242, -22), S(320, -64), S(304, -102), \
}, { \
   S(-116, -142), S(25, -110), S(21, -66), S(-232, -9), S(-53, 7), S(62, -11), S(154, -24), S(2, -39), \
   S(-171, -11), S(-129, 3), S(-250, 22), S(-231, 32), S(52, 35), S(22, 44), S(155, 2), S(-150, 42), \
   S(-250, 7), S(-144, 14), S(-144, 17), S(-282, 46), S(59, 40), S(274, 34), S(228, 32), S(19, 5), \
   S(-193, -6), S(-205, 23), S(-276, 47), S(-325, 65), S(-136, 79), S(-64, 65), S(-69, 45), S(-184, 23), \
   S(-393, 12), S(-238, 2), S(-308, 40), S(-425, 73), S(-192, 97), S(-129, 66), S(-116, 36), S(-151, 13), \
   S(-267, -26), S(-292, 4), S(-329, 28), S(-417, 59), S(-104, 65), S(-102, 52), S(-47, 21), S(-65, -7), \
   S(-160, -79), S(-226, -28), S(-291, 8), S(-338, 26), S(-79, 40), S(-45, 27), S(9, -3), S(19, -34), \
   S(-186, -131), S(-157, -98), S(-182, -63), S(-338, -3), S(-1, -23), S(-44, -3), S(49, -49), S(42, -89), \
}, }

#endif