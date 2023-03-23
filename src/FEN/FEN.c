#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "FEN.h"
#include "movegen.h"
#include "util_macros.h"
#include "string_utils.h"

static double usr_pow(int x, int y) {
    double result = 1;
    for(int i = 0; i < y; i++) {
        result *= x;
    }

    return result;
}

static Color_t CharToColor(char c) {
    if(c == 'w' || c == 'W') {
        return white;
    } else if(c == 'b' || c == 'B') {
        return black;
    } else {
        assert(!"ERROR: Unrecognized Color");
    }
    return 0;
}

static Bitboard_t CalculateSingleBitset(Square_t square) {
    return C64(1) << square;
}

static void UpdateCastlingRights(GameState_t* state, char c) {
    switch (c)
    {
        case 'K':
           SetBits(&(state->castleSquares[white]), white_kingside_castle_bb);
        break;
        case 'Q':
            SetBits(&(state->castleSquares[white]), white_queenside_castle_bb);
        break;
        case 'k':
            SetBits(&(state->castleSquares[black]), black_kingside_castle_bb);
        break;
        case 'q':
            SetBits(&(state->castleSquares[black]), black_queenside_castle_bb);
        break;
        default:
            assert(!"ERROR: Unrecognized Castling");
        break;
    }
}

static Bitboard_t SquareCharsToBitboard(char col, char row) {
    int rowNum = RowCharToInt(row);
    int ColNum = ColCharToInt(col);

    Square_t square = rowNum*8 + ColNum;
    return CalculateSingleBitset(square);
}

static void UpdateEnPassant(FEN_t fen, BoardInfo_t* info,  int* i, GameState_t* state, Color_t color) { 
    if(fen[*i] == '-') {
        (*i)++;
    } else {
        state->enPassantSquare = SquareCharsToBitboard(fen[*i], fen[*i + 1]);
        
        Bitboard_t capturablePawnLocation = (color == white) ? SoutOne(state->enPassantSquare) : NortOne(state->enPassantSquare);
        Bitboard_t eastAdjPawn = info->pawns[color] & EastOne(capturablePawnLocation);
        Bitboard_t westAdjPawn = info->pawns[color] & WestOne(capturablePawnLocation);

        state->canWestEP = eastAdjPawn && EnPassantIsLegal(info, state->enPassantSquare, eastAdjPawn, color);
        state->canEastEP = westAdjPawn && EnPassantIsLegal(info, state->enPassantSquare, westAdjPawn, color);

        (*i) += 2;
    }
}

static void UpdateHalfmoveClock(FEN_t fen, int i, GameState_t* state) {
    HalfmoveCount_t halfmoves = 0;

    int numDigits = 0;
    while(fen[i + numDigits] != ' ' && fen[i + numDigits] != '\0') {
        numDigits++;
    }

    int j = i;
    while(numDigits) {
        halfmoves += usr_pow(10, numDigits-1) * CharToInt(fen[j]);
        numDigits--;
        j++;
    }

    state->halfmoveClock = halfmoves;
}

void InterpretFEN(
    FEN_t fen,
    BoardInfo_t* info,
    GameStack_t* gameStack,
    ZobristStack_t* zobristStack
)
{
    InitBoardInfo(info);
    InitGameStack(gameStack);
    InitZobristStack(zobristStack);

    int rank = 7; // a8 - h8
    int file = 0;

    int i = 0;
    while(fen[i] != ' ') {
        assert(file < 8 && rank >= 0);
        assert(fen[i] != '\0');
        Bitboard_t singleBitset = CalculateSingleBitset(rank*8 + file);

        switch(fen[i])
        {
            case 'R':
                SetBits(&info->rooks[white], singleBitset);
                file++;
                break;
            case 'N':
                SetBits(&info->knights[white], singleBitset);
                file++;
                break;
            case 'B':
                SetBits(&info->bishops[white], singleBitset);
                file++;
                break;
            case 'Q':
                SetBits(&info->queens[white], singleBitset);
                file++;
                break;
            case 'K':
                SetBits(&info->kings[white], singleBitset);
                file++;
                break;
            case 'P':
                SetBits(&info->pawns[white], singleBitset);
                file++;
                break;

            case 'r':
                SetBits(&info->rooks[black], singleBitset);
                file++;
                break;
            case 'n':
                SetBits(&info->knights[black], singleBitset);
                file++;
                break;
            case 'b':
                SetBits(&info->bishops[black], singleBitset);
                file++;
                break;
            case 'q':
                SetBits(&info->queens[black], singleBitset);
                file++;
                break;
            case 'k':
                SetBits(&info->kings[black], singleBitset);
                file++;
                break;
            case 'p':
                SetBits(&info->pawns[black], singleBitset);
                file++;
                break;

            case '/':
                rank--;
                file = 0;
                break;

            default: // some integer
                int emptySpaces = CharToInt(fen[i]);
                assert(emptySpaces >= 1 && emptySpaces <= 8);
                file += emptySpaces;
                break;
        }

        if(file >= 8) {
            file = 0;
        }
        
        i++;
    }

    UpdateAllPieces(info);
    UpdateEmpty(info);
    TranslateBitboardsToMailbox(info);

    i++;
    info->colorToMove = CharToColor(fen[i]);

    i += 2;
    GameState_t* gameState = GetEmptyNextGameState(gameStack);
    if(fen[i] != '-') {
        while (fen[i] != ' ')
        {
            UpdateCastlingRights(gameState, fen[i]);
            i++;
        }   
    } else {
        i++;
    }

    i++;
    UpdateEnPassant(fen, info, &i, gameState, info->colorToMove);

    i++;
    UpdateHalfmoveClock(fen, i, gameState);

    gameState->boardInfo = *info;

    AddZobristHashToStack(zobristStack, HashPosition(info, gameStack));
}

void BoardToFEN(
    BoardInfo_t* info,
    GameStack_t* gameStack,
    char result[2000]
)
{
    int rank = 7; // a8 - h8
    int file = 0;
    int blankSpaces = 0;

    int i = 0;
    while (true) { // probably bad practice but I'm fed up with life.
        Square_t s = rank*8 + file;

        Piece_t piece = PieceOnSquare(info, s);
        Color_t color = ColorOfPiece(info, s); // might not be a piece but whatever

        if(piece == none_type) {
            blankSpaces++;
        } else if(blankSpaces > 0) {
            result[i++] = IntToChar(blankSpaces);
            blankSpaces = 0;
        }

        switch(piece) {
            case pawn:
                result[i++] = (color == white) ? 'P' : 'p';
                break;
            case king:
                result[i++] = (color == white) ? 'K' : 'k';
                break;
            case queen:
                result[i++] = (color == white) ? 'Q' : 'q';
                break;
            case knight:
                result[i++] = (color == white) ? 'N' : 'n';
                break;        
            case rook:
                result[i++] = (color == white) ? 'R' : 'r';
                break;
            case bishop:
                result[i++] = (color == white) ? 'B' : 'b';
                break;      
        }

        file++;
        if(file > 7) {
            file = 0;
            rank--;
            if(blankSpaces > 0) {
                result[i++] = IntToChar(blankSpaces);
                blankSpaces = 0;
            }

            if(rank >= 0) {
                result[i++] = '/';
            } else {
                result[i++] = ' ';
                break;
            }
        }
    }

    result[i++] = (info->colorToMove == white) ? 'w' : 'b';
    result[i++] = ' ';

    Bitboard_t whiteCastle = ReadCastleSquares(gameStack, white); // borgar
    Bitboard_t blackCastle = ReadCastleSquares(gameStack, black);
    if(whiteCastle || blackCastle) {
        if(whiteCastle & white_kingside_castle_bb) {
            result[i++] = 'K';
        }
        if(whiteCastle & white_queenside_castle_bb) {
            result[i++] = 'Q';
        }
        if(blackCastle & black_kingside_castle_bb) {
            result[i++] = 'k';
        }
        if(blackCastle & black_queenside_castle_bb) {
            result[i++] = 'q';
        }
    } else {
        result[i++] = '-';
    }

    result[i++] = ' ';

    if(ReadEnPassant(gameStack)) {
        Square_t epSquare = LSB(ReadEnPassant(gameStack));
        char epString[3];
        SquareToString(epSquare, epString);

        result[i++] = epString[0];
        result[i++] = epString[1];
    } else {
        result[i++] = '-';
    }

    result[i++] = ' ';

    HalfmoveCount_t halfmoves = ReadHalfmoveClock(gameStack);
    char halfmoveBuffer[100];
    sprintf(halfmoveBuffer, "%d", halfmoves);
    int ind = 0;
    while(halfmoveBuffer[ind] != '\0') {
        result[i++] = halfmoveBuffer[ind];
        ind++;
    }

    result[i++] = ' ';

    result[i++] = '0';
    result[i++] = '\0';
}

void PrintFEN(BoardInfo_t* info, GameStack_t* gameStack) {
    char result[2000];
    BoardToFEN(info, gameStack, result);
    printf("%s\n", result);
}

bool FENsMatch(FEN_t expected, FEN_t actual) {
    int len = strlen(expected);
    int finalIndex = len-1;
    while(actual[finalIndex] != ' ') {
        finalIndex--;
    }

    for(int i = 0; i < finalIndex; i++) {
        if(actual[i] != expected[i]) {
            printf("Expected %s, got %s\n", expected, actual);
            return false;
        }
    }
    return true;
}
