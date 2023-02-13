#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "UCI.h"
#include "bitboards.h"
#include "zobrist.h"
#include "FEN.h"
#include "movegen.h"

#define BUFFER_SIZE 256

#define ENGINE_ID "id name Apotheosis\nid author Spamdrew\n"
#define UCI_OK "uciok\n"
#define READY_OK "readyok\n"

#define STARTPOS "startpos"

typedef uint8_t UciSignal_t;
enum {
    signal_invalid,
    signal_uci,
    signal_is_ready,
    signal_quit,
    signal_new_game,
    signal_position
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

static bool StringsMatch(const char* s1, const char* s2) {
    return !strcmp(s1, s2);
}

static void SkipToNextCharacter(char input[BUFFER_SIZE], int* i) {
    while(input[*i] == ' ') {
        (*i)++;
    }
}

static UciSignal_t InterpretWord(const char* word) {
    if (StringsMatch(word, "uci")) {
        return signal_uci;
    } else if(StringsMatch(word, "isready")) {
        return signal_is_ready;
    } else if(StringsMatch(word, "quit")) {
        return signal_quit;
    } else if(StringsMatch(word, "ucinewgame")) {
        return signal_new_game;
    } else if(StringsMatch(word, "position")) {
        return signal_position;
    }

    return signal_invalid;
}

static bool ContainsStartPos(char input[BUFFER_SIZE], int i) {
    char first8Letters[9];
    for(int j = 0; j < strlen(STARTPOS); j++) {
        first8Letters[j] = input[i];
        i++;
    }
    first8Letters[8] = '\0';

    return StringsMatch(first8Letters, STARTPOS);
}

static MoveList_t ParseMoves(char input[BUFFER_SIZE], int* i) {
    MoveList_t moveList;
    moveList.maxIndex = movelist_empty;

    // assumes this is the last thing in the string.
    while(input[*i] != ' ' && input[*i] != '\0') {

    }

    return moveList;
}

static void InterpretPosition(
    char input[BUFFER_SIZE],
    int* i,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    char fenString[BUFFER_SIZE];
    int fenStringIndex = 0;

    Color_t colorToMove;
    if(ContainsStartPos(input, *i)) {
        colorToMove = InterpretFEN(START_FEN, boardInfo, gameStack, zobristStack);
        *i += strlen(STARTPOS);  
    } else {
        while(input[*i] != 'm' || input[*i] != '\0') {
            fenString[fenStringIndex] = input[*i];
            (*i)++;
            fenStringIndex++;
        }
        fenString[fenStringIndex] = '\0';

        colorToMove = InterpretFEN(fenString, boardInfo, gameStack, zobristStack);
    }
    SkipToNextCharacter(input, i);
    // MoveList_t moveList = ParseMoves(input, i);
}

static bool RespondToSignal(
    char input[BUFFER_SIZE],
    int* i,
    UciSignal_t signal,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    switch(signal) {
    case signal_uci:
        printf(ENGINE_ID);
        printf(UCI_OK);
        break;
    case signal_is_ready:
        printf(READY_OK);
        break;
    case signal_quit:
        return false;
    case signal_new_game:
        // TODO
        break;
    case signal_position:
        InterpretPosition(input, i, boardInfo, gameStack, zobristStack);
        break;
    default:
        break;
    }

    return true;
}

bool InterpretUCIInput() {
    char input[BUFFER_SIZE];
    fgets(input, BUFFER_SIZE, stdin);

    int cWordIndex = 0;
    char currentWord[BUFFER_SIZE];

    int i = 0;
    BoardInfo_t boardinfo;
    GameStack_t gameStack;
    ZobristStack_t zobristStack;
    while(i < BUFFER_SIZE && input[i] != '\0') {
        currentWord[cWordIndex] = input[i];
        cWordIndex++;

        UciSignal_t signal = signal_invalid;
        if(input[i+1] == ' ' || input[i+1] == '\0' || input[i+1] == '\n') {
            currentWord[cWordIndex] = '\0';
            cWordIndex = 0;
            signal = InterpretWord(currentWord);
        }

        i++;
        SkipToNextCharacter(input, &i);
        bool keepRunning = RespondToSignal(input, &i, signal, &boardinfo, &gameStack, &zobristStack);
        if(!keepRunning) {
            return false; // quit immediately
        }
    }

    return true; // true means application keeps running
}