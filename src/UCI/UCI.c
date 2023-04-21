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
#include "time_constants.h"
#include "util_macros.h"
#include "tuner.h"
#include "datagen.h"
#include "string_utils.h"

#define BUFFER_SIZE 50000

#define ENGINE_ID "id name Apotheosis v3.0.0\nid author Andrew Hockman\n"
#define UCI_OK "uciok\n"
#define READY_OK "readyok\n"

// UCI Options
#define OVERHEAD "Overhead"
#define HASH "Hash"

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
    signal_go,
    signal_setoption,

    signal_print_board,
    signal_print_static_eval,
    signal_begin_datagen,
    signal_begin_tuning,
};
 
void UciApplicationDataInit(UciApplicationData_t* data) {
    InterpretFEN(START_FEN, &data->boardInfo, &data->gameStack, &data->zobristStack);
    UciSearchInfoInit(&data->uciSearchInfo);
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

static void SkipToNextCharacter(char input[BUFFER_SIZE], int* i) {
    while(input[*i] == ' ' || input[*i] == '\n') {
        (*i)++;
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

    SkipToNextCharacter(input, i);
}

static void SkipNextWord(char input[BUFFER_SIZE], int* i) {
    SkipToNextCharacter(input, i);
    while(input[*i] != '\n' && input[*i] != ' ' && input[*i] != '\0') {
        (*i)++;
    }
    SkipToNextCharacter(input, i);
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
    } else if (StringsMatch(word, "setoption")) {
        return signal_setoption; 

    // my options below this point
    } else if(StringsMatch(word, "evaluate")) {
        return signal_print_static_eval;
    } else if(StringsMatch(word, "board")) {
        return signal_print_board;
    } else if (StringsMatch(word, "datagen")) {
        return signal_begin_datagen;
    } else if (StringsMatch(word, "tuning")) {
        return signal_begin_tuning;
    }

    return signal_invalid;
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

    char command[BUFFER_SIZE];
    GetNextWord(input, command, i);

    if(StringsMatch(command, "startpos")) {
        InterpretFEN(START_FEN, boardInfo, gameStack, zobristStack);
    } else if (StringsMatch(command, "fen")) {
        while(input[*i] != 'm' && input[*i] != '\0') {
            fenString[fenStringIndex] = input[*i];
            (*i)++;
            fenStringIndex++;
        }
        fenString[fenStringIndex] = '\0';

        InterpretFEN(fenString, boardInfo, gameStack, zobristStack);
    }

    SkipNextWord(input, i);

    ParseAndPlayMoves(input, i, boardInfo, gameStack, zobristStack);
}

uint32_t NumberStringToNumber(const char* numString) {
    int len = strlen(numString);

    Milliseconds_t result = 0;
    int multiplier = 1;
    for(int i = (len-1); i >=0; i--) {
        result += CharToInt(numString[i]) * multiplier;
        multiplier *= 10;
    }

    return result;
}

static void GetSearchResults(UciSearchInfo_t* uciSearchInfo, UciApplicationData_t* applicationData)
{
    Search(
        uciSearchInfo,
        &applicationData->boardInfo,
        &applicationData->gameStack,
        &applicationData->zobristStack,
        true
    );
}

void InterpretGoArguments(char input[BUFFER_SIZE], int* i, UciSearchInfo_t* searchInfo) {
    UciSearchInfoTimeInfoReset(searchInfo);

    char nextWord[BUFFER_SIZE];

    while(input[*i] != '\0') {
        GetNextWord(input, nextWord, i);

        if(StringsMatch(nextWord, "wtime")) {
            GetNextWord(input, nextWord, i);
            searchInfo->wTime = NumberStringToNumber(nextWord);

        } else if(StringsMatch(nextWord, "btime")) {
            GetNextWord(input, nextWord, i);
            searchInfo->bTime = NumberStringToNumber(nextWord);
            
        } else if(StringsMatch(nextWord, "winc")) {
            GetNextWord(input, nextWord, i);
            searchInfo->wInc = NumberStringToNumber(nextWord);
            
        } else if(StringsMatch(nextWord, "binc")) {
            GetNextWord(input, nextWord, i);
            searchInfo->bInc = NumberStringToNumber(nextWord);    

        } else if(StringsMatch(nextWord, "infinite")) {
            searchInfo->wTime = MSEC_MAX;       
            searchInfo->bTime = MSEC_MAX;

        } else if(StringsMatch(nextWord, "depth")) {
            GetNextWord(input, nextWord, i);
            searchInfo->depthLimit = NumberStringToNumber(nextWord);

            if(searchInfo->wTime == 0 || searchInfo->bTime == 0) {
                searchInfo->wTime = MSEC_MAX;       
                searchInfo->bTime = MSEC_MAX;
            }

        } else if(StringsMatch(nextWord, "movetime")) {
            GetNextWord(input, nextWord, i);       
            searchInfo->forceTime = NumberStringToNumber(nextWord); 
        }
    }
}

#define SendUciOption(name, type, formatString, ...) \
do { \
    printf("option name %s type %s ", name, type); \
    printf(formatString, __VA_ARGS__); \
    printf("\n"); \
} while(0)

static void UciSignalResponse() {
    printf(ENGINE_ID);
    SendUciOption(OVERHEAD, "spin", "default %d min %d max %d", overhead_default_msec, overhead_min_msec, overhead_max_msec);
    SendUciOption(HASH, "spin", "default %d min %d max %d", hash_default_mb, hash_min_mb, hash_max_mb);
    printf(UCI_OK);
}

static void SetOption(char input[BUFFER_SIZE], int* i, UciSearchInfo_t* searchInfo) {
    char nextWord[BUFFER_SIZE];
    GetNextWord(input, nextWord, i);

    if(!StringsMatch(nextWord, "name")) {
        return;
    }
    GetNextWord(input, nextWord, i);

    if(StringsMatch(nextWord, OVERHEAD)) {
        SkipNextWord(input, i);

        GetNextWord(input, nextWord, i);
        searchInfo->overhead = NumberStringToNumber(nextWord);
        CLAMP_TO_RANGE(searchInfo->overhead, overhead_min_msec, overhead_max_msec);
    } else if (StringsMatch(nextWord, HASH)) {
        SkipNextWord(input, i);

        GetNextWord(input, nextWord, i);
        Megabytes_t hashSize = NumberStringToNumber(nextWord);
        CLAMP_TO_RANGE(hashSize, overhead_min_msec, overhead_max_msec);
        TeardownTT(&searchInfo->tt);
        TranspositionTableInit(&searchInfo->tt, hashSize);
    }
}

static bool RespondToSignal(
    char input[BUFFER_SIZE],
    int* i,
    UciSignal_t signal,
    UciApplicationData_t* applicationData
)
{
    char outputFile[BUFFER_SIZE];
    char inputFile[BUFFER_SIZE];
    
    switch(signal) {
    case signal_uci:
        UciSignalResponse();
        break;
    case signal_is_ready:
        printf(READY_OK);
        break;
    case signal_quit:
        return false;
    case signal_new_game:
        ClearTTEntries(&applicationData->uciSearchInfo.tt);
        break;
    case signal_position:
        InterpretPosition(
            input,
            i,
            &applicationData->boardInfo,
            &applicationData->gameStack,
            &applicationData->zobristStack
        );
        break;
    case signal_go:
        InterpretGoArguments(input, i, &applicationData->uciSearchInfo);
        GetSearchResults(&applicationData->uciSearchInfo, applicationData);
        break;   
    case signal_setoption:
        SetOption(input, i, &applicationData->uciSearchInfo);
        break;

    case signal_print_static_eval:
        printf("%d cp\n", ScoreOfPosition(&applicationData->boardInfo));
        break;    
    case signal_print_board:
        PrintChessboard(&applicationData->boardInfo);
        printf("\n");
        PrintFEN(&applicationData->boardInfo, &applicationData->gameStack);
        break;
    case signal_begin_datagen:
        GetNextWord(input, outputFile, i);
        GenerateData(outputFile);
        break;
    case signal_begin_tuning:
        GetNextWord(input, inputFile, i);
        TuneParameters(inputFile);
        break;
    default:
        break;
    }

    return true;
}

bool InterpretUCIInput(UciApplicationData_t* applicationData)
{
    char input[BUFFER_SIZE];
    memset(input, '\0', BUFFER_SIZE* sizeof(char));
    if(fgets(input, BUFFER_SIZE-1, stdin) == NULL) {
        return true;
    }

    char currentWord[BUFFER_SIZE];

    int i = 0;
    while(i < BUFFER_SIZE && input[i] != '\0') {
        GetNextWord(input, currentWord, &i);
        UciSignal_t signal = InterpretWord(currentWord);

        bool keepRunning = RespondToSignal(input, &i, signal, applicationData);
        if(!keepRunning) {
            return false; // quit immediately
        }
    }

    return true; // true means application keeps running
}

void InterpretUCIString(
    BoardInfo_t* boardInfo,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack,
    const char* _input
)
{
    UciSearchInfo_t searchInfo;
    UciSearchInfoInit(&searchInfo);

    UciApplicationData_t data;
    data.boardInfo = *boardInfo;
    data.gameStack = *gameStack;
    data.zobristStack = *zobristStack;
    data.uciSearchInfo = searchInfo;

    char input[BUFFER_SIZE];
    memset(input, '\0', BUFFER_SIZE* sizeof(char));
    memcpy(input, _input, strlen(_input));

    char currentWord[BUFFER_SIZE];

    int i = 0;
    while(i < BUFFER_SIZE && input[i] != '\0') {
        GetNextWord(input, currentWord, &i);
        UciSignal_t signal = InterpretWord(currentWord);

        bool keepRunning = RespondToSignal(input, &i, signal, &data);
        if(!keepRunning) {
            return; 
        }
    }

    *boardInfo = data.boardInfo;
    *gameStack = data.gameStack;
    *zobristStack = data.zobristStack;

    TeardownTT(&data.uciSearchInfo.tt);
}

void SendPvInfo(PvTable_t* pvTable) {
    // assumes this is part of larger info string-
    PvLength_t variationLength = pvTable->pvLength[0];
    printf(" pv");

    for(int i = 0; i < variationLength; i++) {
        Move_t move = pvTable->moveMatrix[0][i];
        printf(" ");
        PrintMove(move, false);
    }
}