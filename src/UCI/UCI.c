#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "UCI.h"
#include "bitboards.h"
#include "FEN.h"
#include "movegen.h"
#include "make_and_unmake.h"
#include "search.h"

#define BUFFER_SIZE 50000

#define ENGINE_ID "id name Apotheosis\nid author Spamdrew\n"
#define UCI_OK "uciok\n"
#define READY_OK "readyok\n"
#define BESTMOVE "bestmove"

#define STARTPOS "startpos"

typedef uint8_t UciSignal_t;
enum {
    signal_invalid,
    signal_uci,
    signal_is_ready,
    signal_quit,
    signal_new_game,
    signal_position,
    signal_go
};

static void UciTimeInfoInit(PlayerTimeInfo_t* uciTimeInfo) {
    uciTimeInfo->wTime = 0;
    uciTimeInfo->bTime = 0;
    uciTimeInfo->wInc = 0;
    uciTimeInfo->bInc = 0;
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

static int NumCharToInt(char numChar) {
    return (int)numChar - 48;
}
 
bool UCITranslateMove(Move_t* move, const char* moveText, BoardInfo_t* boardInfo, GameStack_t* gameStack) {
    int stringLen = strlen(moveText);
    if(stringLen > 5 || stringLen < 4) {
        return false;
    }

    char fromCol = moveText[0];
    char fromRow = moveText[1];
    char toCol = moveText[2];
    char toRow = moveText[3];

    Square_t fromSquare = RowCharToInt(fromRow)*8 + ColCharToInt(fromCol);
    Square_t toSquare = RowCharToInt(toRow)*8 + ColCharToInt(toCol);

    InitMove(move);
    WriteFromSquare(move, fromSquare);
    WriteToSquare(move, toSquare);

    if(stringLen == 5) {
        char promotionType = moveText[4];
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
        }
    }

    Piece_t type = PieceOnSquare(boardInfo, fromSquare);

    if(type == king && (abs(fromSquare%8 - toSquare%8) >= 2)) {
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

static void MoveStructToUciString(Move_t move, char moveString[BUFFER_SIZE]) {
    memset(moveString, '\0', BUFFER_SIZE* sizeof(char));

    Square_t fromSquare = ReadFromSquare(move);
    int fromRow = fromSquare / 8;
    int fromCol = fromSquare % 8;

    Square_t toSquare = ReadToSquare(move);
    int toRow = toSquare / 8;
    int toCol = toSquare % 8;

    moveString[0] = ColToLetterChar(fromCol);
    moveString[1] = RowToNumberChar(fromRow);
    moveString[2] = ColToLetterChar(toCol);
    moveString[3] = RowToNumberChar(toRow);
    
    if(ReadSpecialFlag(move) == promotion_flag) {
        switch(ReadPromotionPiece(move))
        {
            case knight:
                moveString[4] = 'n';
                break;
            case bishop:
                moveString[4] = 'b';
                break;
            case rook:
                moveString[4] = 'r';
                break;
            case queen:
                moveString[4] = 'q';
                break;
            default:
                break;
        }
    }
}

static void GetNextWord(char input[BUFFER_SIZE], char nextWord[BUFFER_SIZE], int* i) {
    memset(nextWord, '\0', BUFFER_SIZE* sizeof(char));

    int wordIndex = 0;
    while(input[*i] != '\n' && input[*i] != ' ' && input[*i] != '\0') {
        nextWord[wordIndex] = input[*i];
        wordIndex++;
        (*i)++;
    }
}

static bool StringsMatch(const char* s1, const char* s2) {
    return !strcmp(s1, s2);
}

static void SkipToNextCharacter(char input[BUFFER_SIZE], int* i) {
    while(input[*i] == ' ' || input[*i] == '\n') {
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
    } else if(StringsMatch(word, "go")) {
        return signal_go;
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

static bool IsLetter(char c) {
    int asciiVal = (int)c;
    return 
        ((asciiVal >= 'a') && (asciiVal <= 'z'));
}

static bool IsRowNumber(char c) {
    int asciiVal = (int)c;
    return  (asciiVal >= 49) && (asciiVal <= 56);
}

static void ParseAndPlayMoves(
    char input[BUFFER_SIZE],
    int* i,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    char moveBuffer[BUFFER_SIZE];
    size_t moveBufferSize = BUFFER_SIZE * sizeof(char);

    // assumes this is the last thing in the string.
    while(input[*i] != '\n' && input[*i] != '\0') {
        memset(moveBuffer, '\0', moveBufferSize);
        int moveBufferIndex = 0;
        SkipToNextCharacter(input, i);

        while(IsRowNumber(input[*i]) || IsLetter(input[*i])) {
            moveBuffer[moveBufferIndex] = input[*i];
            moveBufferIndex++;
            (*i)++;
        }

        Move_t move;
        InitMove(&move);
        if(UCITranslateMove(&move, moveBuffer, boardInfo, gameStack)) {
            MakeMove(boardInfo, gameStack, move);
            AddZobristHashToStack(zobristStack, HashPosition(boardInfo, gameStack));
        }
    }
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

    if(ContainsStartPos(input, *i)) {
        InterpretFEN(START_FEN, boardInfo, gameStack, zobristStack);
        *i += strlen(STARTPOS);  
    } else {
        while(input[*i] != 'm' && input[*i] != '\0') {
            fenString[fenStringIndex] = input[*i];
            (*i)++;
            fenStringIndex++;
        }
        fenString[fenStringIndex] = '\0';

        InterpretFEN(fenString, boardInfo, gameStack, zobristStack);
    }

    SkipToNextCharacter(input, i);
    while(input[*i] != ' ' && input[*i] != '\0') {
        (*i)++; // skips past the "moves" command which I assume is there
    }
    SkipToNextCharacter(input, i);

    ParseAndPlayMoves(input, i, boardInfo, gameStack, zobristStack);
}

Milliseconds_t TimeStringToNumber(const char* numString) {
    int len = strlen(numString);

    Milliseconds_t result = 0;
    int multiplier = 1;
    for(int i = (len-1); i >=0; i--) {
        result += NumCharToInt(numString[i]) * multiplier;
        multiplier *= 10;
    }

    return result;
}

static void SendNumericalInfoCommand(const char* command, int data) {
    printf("info %s %d\n", command, data);
}

static void GetSearchResults(
    PlayerTimeInfo_t uciTimeInfo,
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    SearchResults_t searchResults = 
        Search(
            uciTimeInfo,
            boardInfo,
            gameStack,
            zobristStack,
            6
        );

    char moveString[BUFFER_SIZE];
    MoveStructToUciString(searchResults.bestMove, moveString);

    SendNumericalInfoCommand("score", searchResults.score);

    printf(BESTMOVE);
    printf(" %s\n", moveString);
}

PlayerTimeInfo_t InterpretGoArguements(char input[BUFFER_SIZE], int* i) {
    PlayerTimeInfo_t timeInfo;
    UciTimeInfoInit(&timeInfo);

    char nextWord[BUFFER_SIZE];
    while(input[*i] != '\0') {
        GetNextWord(input, nextWord, i);
        SkipToNextCharacter(input, i);

        if(StringsMatch(nextWord, "wtime")) {
            GetNextWord(input, nextWord, i);
            timeInfo.wTime = TimeStringToNumber(nextWord);

        } else if(StringsMatch(nextWord, "btime")) {
            GetNextWord(input, nextWord, i);
            timeInfo.bTime = TimeStringToNumber(nextWord);
            
        } else if(StringsMatch(nextWord, "winc")) {
            GetNextWord(input, nextWord, i);
            timeInfo.wInc = TimeStringToNumber(nextWord);
            
        } else if(StringsMatch(nextWord, "binc")) {
            GetNextWord(input, nextWord, i);
            timeInfo.bInc = TimeStringToNumber(nextWord);       
        }
    }

    return timeInfo;
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
    case signal_go:
        PlayerTimeInfo_t uciTimeInfo = InterpretGoArguements(input, i);
        GetSearchResults(uciTimeInfo, boardInfo, gameStack, zobristStack);
        break;     
    default:
        break;
    }

    return true;
}

bool InterpretUCIInput(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    char input[BUFFER_SIZE];
    memset(input, '\0', BUFFER_SIZE* sizeof(char));
    fgets(input, BUFFER_SIZE, stdin);

    char currentWord[BUFFER_SIZE];

    int i = 0;
    while(i < BUFFER_SIZE && input[i] != '\0') {
        GetNextWord(input, currentWord, &i);
        UciSignal_t signal = InterpretWord(currentWord);

        SkipToNextCharacter(input, &i);

        bool keepRunning = RespondToSignal(input, &i, signal, boardInfo, gameStack, zobristStack);
        if(!keepRunning) {
            return false; // quit immediately
        }
    }

    return true; // true means application keeps running
}