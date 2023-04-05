#include "eval_helpers.h"

Bitboard_t WhiteFill(Bitboard_t b) {
    b |= GenShiftNorth(b, 1);
    b |= GenShiftNorth(b, 2);
    b |= GenShiftNorth(b, 4);
    return b;
}

Bitboard_t BlackFill(Bitboard_t b) {
    b |= GenShiftSouth(b, 1);
    b |= GenShiftSouth(b, 2);
    b |= GenShiftSouth(b, 4);
    return b;
}

Bitboard_t WhiteForwardFill(Bitboard_t b) {
    return NortOne(WhiteFill(b));
}

Bitboard_t BlackForwardFill(Bitboard_t b) {
    return SoutOne(BlackFill(b));
}