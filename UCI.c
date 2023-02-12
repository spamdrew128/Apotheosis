#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "UCI.h"
#include "bitboards.h"

#define BUFFER_SIZE 256

typedef uint8_t UciSignal_t;
enum {
    signal_invalid,
    signal_uci
};

static char RowCharToNumber(char row) {
    return (int)row - 49;
}

static char ColCharToNumber(char col) {
    return (int)col - 97;
}

bool UCITranslateMove(Move_t* move, const char* moveText, BoardInfo_t* boardInfo, GameStack_t* gameStack) {
    int stringLen = strlen(moveText);
    if(stringLen > 5 || stringLen < 4) {
        printf("Invalid move format\n");
        return false;
    }

    char fromCol = moveText[0];
    char fromRow = moveText[1];
    char toCol = moveText[2];
    char toRow = moveText[3];

    Square_t fromSquare = RowCharToNumber(fromRow)*8 + ColCharToNumber(fromCol);
    Square_t toSquare = RowCharToNumber(toRow)*8 + ColCharToNumber(toCol);

    InitMove(move);
    WriteFromSquare(move, fromSquare);
    WriteToSquare(move, toSquare);

    if(stringLen == 5) {
        char promotionType = moveText[3];
        if(promotionType == 'n' || promotionType == 'N') {
            WritePromotionPiece(move, knight);
            WriteSpecialFlag(move, promotion_flag);
        } else if(promotionType == 'b' || promotionType == 'B') {
            WritePromotionPiece(move, bishop);
            WriteSpecialFlag(move, promotion_flag);
        } else if(promotionType == 'r' || promotionType == 'R') {
            WritePromotionPiece(move, rook);
            WriteSpecialFlag(move, promotion_flag);
        } else if(promotionType == 'q' || promotionType == 'Q') {
            WritePromotionPiece(move, queen);
            WriteSpecialFlag(move, promotion_flag);
        } else {
            printf("Invalid promotion specifier\n");
            return false;
        }
    }

    Piece_t type = PieceOnSquare(boardInfo, fromSquare);

    if(type == king && (abs(fromSquare - toSquare) >= 2)) {
        WriteSpecialFlag(move, castle_flag);
    }

    Bitboard_t enPassantBB = ReadEnPassant(gameStack);
    if(type == pawn && enPassantBB) { 
        if(LSB(enPassantBB) == toSquare) {
            WriteSpecialFlag(move, en_passant_flag);
        }
    }

    return true;
}

static bool IdenticalStrings(const char* s1, const char* s2) {
    return !strcmp(s1, s2);
}

static UciSignal_t InterpretWord(const char* word) {
    if(IdenticalStrings(word, "uci")) {
        return signal_uci;
    }

    return signal_invalid;
}

void InterpretUCIInput() {
    char input[BUFFER_SIZE];
    fgets(input, BUFFER_SIZE, stdin);

    int cWordIndex = 0;
    char currentWord[BUFFER_SIZE];

    int i = 0;
    while(i < BUFFER_SIZE && input[i] != '\0') {
        currentWord[cWordIndex] = input[i];
        cWordIndex++;

        if(input[i+1] == ' ' || input[i+1] == '\0' || input[i+1] == '\n') {
            currentWord[cWordIndex] = '\0';
            cWordIndex = 0;
            UciSignal_t signal = InterpretWord(currentWord);
            printf("%d\n", signal);
        }

        i++;
    }
}