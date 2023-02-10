#ifndef __CASTLING_H__
#define __CASTLING_H__

#include <stdint.h>
#include <stdbool.h>

#include "board_constants.h"

typedef struct {
    uint8_t data;
} CastleRights_t;

void InitCastleRightsToZero(CastleRights_t* castleRights);

void SetKingsideCastleRights(CastleRights_t* castleRights, Color_t color);

void SetQueensideCastleRights(CastleRights_t* castleRights, Color_t color);

void ResetKingsideCastleRights(CastleRights_t* castleRights, Color_t color);

void ResetQueensideCastleRights(CastleRights_t* castleRights, Color_t color);

void ResetAllRights(CastleRights_t* castleRights, Color_t color);

bool ReadKingsideCastleRights(CastleRights_t castleRights, Color_t color);

bool ReadQueensideCastleRights(CastleRights_t castleRights, Color_t color);

#endif