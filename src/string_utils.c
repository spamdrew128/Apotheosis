#include "string_utils.h"

bool StringsMatch(const char* s1, const char* s2) {
    return !strcmp(s1, s2);
}

static char RowToNumberChar(int row) {
    return (char)(row + 49);
}

static char ColToLetterChar(int col) {
    return (char)(col + 97);
}

static int RowCharToInt(char row) {
    return (int)row - 49;
}

static int ColCharToInt(char col) {
    return (int)col - 97;
}

static int CharToInt(char c) {
    return (int)c - 48;
}

static char RowToNumber(int row) {
    return (char)(row + 49);
}

static char ColToLetter(int col) {
    return (char)(col + 97);
}

static void SquareToString(Square_t square, char string[3]) {
    int row = square / 8;
    int col = square % 8;

    string[0] = ColToLetter(col);
    string[1] = RowToNumber(row);
    string[2] = '\0';
}