#include "castling.h"

enum {
    white_ks_bit = 0b0001,
    black_ks_bit = 0b0010,
    white_qs_bit = 0b0100,
    black_qs_bit = 0b1000,

    white_all_bits = 0b0101,
    black_all_bits = 0b1010
};

void InitCastleRightsToZero(CastleRights_t* castleRights) {
    castleRights->data = 0;
}

void SetKingsideCastleRights(CastleRights_t* castleRights, Color_t color) {
    castleRights->data |= (white_ks_bit << color);
}

void SetQueensideCastleRights(CastleRights_t* castleRights, Color_t color) {
    castleRights->data |= (white_qs_bit << color);
}

void ResetKingsideCastleRights(CastleRights_t* castleRights, Color_t color) {
    castleRights->data &= ~(white_ks_bit << color);
}

void ResetQueensideCastleRights(CastleRights_t* castleRights, Color_t color) {
    castleRights->data &= ~(white_qs_bit << color);
}

void ResetAllRights(CastleRights_t* castleRights, Color_t color) {
    castleRights->data &= ~(white_all_bits << color);
}

bool ReadKingsideCastleRights(CastleRights_t castleRights, Color_t color) {
    return castleRights.data & (white_ks_bit << color);
}

bool ReadQueensideCastleRights(CastleRights_t castleRights, Color_t color) {
    return castleRights.data & (white_qs_bit << color);
}