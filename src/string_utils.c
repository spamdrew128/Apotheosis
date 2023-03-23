#include <string.h>

#include "string_utils.h"

bool StringsMatch(const char* s1, const char* s2) {
    return !strcmp(s1, s2);
}

char RowToNumberChar(int row) {
    return (char)(row + 49);
}

char ColToLetterChar(int col) {
    return (char)(col + 97);
}

int RowCharToInt(char row) {
    return (int)row - 49;
}

int ColCharToInt(char col) {
    return (int)col - 97;
}

int CharToInt(char c) {
    return (int)c - 48;
}

char IntToChar(int n) {
    return (char) (n + 48);
}

void SquareToString(Square_t square, char string[3]) {
    int row = square / 8;
    int col = square % 8;

    string[0] = ColToLetterChar(col);
    string[1] = RowToNumberChar(row);
    string[2] = '\0';
}

char PieceToChar(Piece_t piece) {
    switch (piece)
    {
        case knight:
            return 'n';
        case bishop:
            return 'b';
        case king:
            return 'k';
        case rook:
            return 'r';
        case pawn:
            return 'p';
        case queen:
            return 'q';
    }

    return ' ';
}