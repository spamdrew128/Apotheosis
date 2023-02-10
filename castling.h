#ifndef __CASTLING_H__
#define __CASTLING_H__

#include <stdint.h>
#include <stdbool.h>

#include "board_constants.h"

typedef struct {
    uint8_t data;
} CastleRights_t;

void InitCastleRights(CastleRights_t* castleRights);

void WriteKingsideCastleRights(CastleRights_t* castleRights, Color_t color);

void WriteQueensideCastleRights(CastleRights_t* castleRights, Color_t color);

bool ReadKingsideCastleRights(CastleRights_t castleRights, Color_t color);

bool ReadQueensideCastleRights(CastleRights_t castleRights, Color_t color);

#endif