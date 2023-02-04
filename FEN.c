#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "FEN.h"
#include "lookup.h"
#include "board_constants.h"
#include "game_state_old.h"

static int CharToInt(char c) {
    return ((int) c) - 48;
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

static void UpdateCastlingRights(GameStateOld_t* state, char c) {
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

static char RowCharToNumber(char row) {
    return (int)row - 49;
}

static char ColCharToNumber(char col) {
    return (int)col - 97;
}

static Bitboard_t SquareCharsToBitboard(char col, char row) {
    int rowNum = RowCharToNumber(row);
    int ColNum = ColCharToNumber(col);

    Square_t square = rowNum*8 + ColNum;
    return GetSingleBitset(square);
}

static void UpdateEnPassant(FEN_t fen, int* i, GameStateOld_t* state) {
    if(fen[*i] == '-') {
        (*i)++;
    } else {
        state->enPassantSquares = SquareCharsToBitboard(fen[*i], fen[*i + 1]);
        (*i) += 2;
    }
}

static void UpdateHalfmoveClock(FEN_t fen, int i, GameStateOld_t* state) {
    HalfmoveCount_t halfmoves = 0;

    int numDigits = 0;
    while(fen[i + numDigits] != ' ' && fen[i + numDigits] != '\0') {
        numDigits++;
    }

    int j = i;
    while(numDigits) {
        halfmoves += (int)pow(10, numDigits-1) * CharToInt(fen[j]);
        numDigits--;
        j++;
    }

    state->halfmoveClock = halfmoves;
}

Color_t InterpretFEN(FEN_t fen, BoardInfo_t* info) {
    InitBoardInfo(info);

    int rank = 7; // a8 - h8
    int file = 0;

    int i = 0;
    while(fen[i] != ' ') {
        assert(file < 8 && rank >= 0);
        assert(fen[i] != '\0');
        Bitboard_t singleBitset = GetSingleBitset(rank*8 + file);

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
    Color_t colorToMove = CharToColor(fen[i]);

    i += 2;
    GameStateOld_t* gameState = GetEmptyNextGameStateOld();
    while (fen[i] != ' ')
    {
        UpdateCastlingRights(gameState, fen[i]);
        i++;
    }   

    i++;
    UpdateEnPassant(fen, &i, gameState);

    i++;
    UpdateHalfmoveClock(fen, i, gameState);

    return colorToMove;
}
