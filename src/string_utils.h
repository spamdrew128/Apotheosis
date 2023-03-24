#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <stdbool.h>

#include "board_constants.h"

bool StringsMatch(const char* s1, const char* s2);

char RowToNumberChar(int row);

char ColToLetterChar(int col);

int RowCharToInt(char row);

int ColCharToInt(char col);

int CharToInt(char c);

char IntToChar(int n);

void SquareToString(Square_t square, char string[3]);

char PieceToChar(Piece_t piece);

#endif