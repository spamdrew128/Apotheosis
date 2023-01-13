#include <stdio.h>
#include <assert.h>

#include "FEN.h"
#include "lookup.h"
#include "board_constants.h"

static int CharToInt(char c) {
    return ((int) c) - 48;
}

void InterpretFEN(FEN_t fen, BoardInfo_t* info) {
    int rank = 7; // a8 - h8
    int file = 0;

    int i = 0;
    while(fen[i] != '\0') {
        assert(file < 8 && rank >= 0);
        Bitboard_t singleBitset = GetSingleBitset(rank*8 + file);

        switch(fen[i])
        {
            case 'R':
                SetBits(info->rooks[white], singleBitset);
                file++;
                break;
            case 'N':
                SetBits(info->knights[white], singleBitset);
                file++;
                break;
            case 'B':
                SetBits(info->bishops[white], singleBitset);
                file++;
                break;
            case 'Q':
                SetBits(info->queens[white], singleBitset);
                file++;
                break;
            case 'K':
                SetBits(info->kings[white], singleBitset);
                file++;
                break;
            case 'P':
                SetBits(info->pawns[white], singleBitset);
                file++;
                break;

            case 'r':
                SetBits(info->rooks[black], singleBitset);
                file++;
                break;
            case 'n':
                SetBits(info->knights[black], singleBitset);
                file++;
                break;
            case 'b':
                SetBits(info->bishops[black], singleBitset);
                file++;
                break;
            case 'q':
                SetBits(info->queens[black], singleBitset);
                file++;
                break;
            case 'k':
                SetBits(info->kings[black], singleBitset);
                file++;
                break;
            case 'p':
                SetBits(info->pawns[black], singleBitset);
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

    for(int i = 0; i < 2; i++) {
        info->allPieces[i] = 
            info->pawns[i] |
            info->knights[i] |
            info->bishops[i] |
            info->rooks[i] |
            info->queens[i] |
            info->kings[i];
    }

    info->empty = ~(info->allPieces[white] | info->allPieces[black]);
}
