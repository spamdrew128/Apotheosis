#ifndef __FEN_H__
#define __FEN_H__

#include "bitboards.h"
#include "board_info.h"

typedef const char* FEN_t;

void InterpretFEN(FEN_t fen, BoardInfo_t* info);

#endif