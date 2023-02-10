#include "castling.h"

enum {
    black_ks_bit = 0b0001,
    black_qs_bit = 0b0010,
    white_ks_bit = 0b0100,
    white_qs_bit = 0b1000
};

void InitCastleRights(CastleRights_t* castleRights) {
    castleRights->data = 0;
}

void WriteKingsideCastleRights(CastleRights_t* castleRights, Color_t color) {
    switch (color) {
    case white:
        castleRights->data |= white_ks_bit;
        break;
    case black:
        castleRights->data |= black_ks_bit;
        break;
    }
}

void WriteQueensideCastleRights(CastleRights_t* castleRights, Color_t color) {
    switch (color) {
    case white:
        castleRights->data |= white_qs_bit;
        break;
    case black:
        castleRights->data |= black_qs_bit;
        break;
    }
}

bool ReadKingsideCastleRights(CastleRights_t castleRights, Color_t color) {
    switch (color) {
    case white:
        return castleRights.data & white_ks_bit;

    case black:
        return castleRights.data & black_ks_bit;
    }
}

bool ReadQueensideCastleRights(CastleRights_t castleRights, Color_t color) {
    switch (color) {
    case white:
        return castleRights.data & white_qs_bit;

    case black:
        return castleRights.data & black_qs_bit;
    }
}