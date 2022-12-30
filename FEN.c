#include <stdio.h>
#include <assert.h>

#include "FEN.h"
#include "lookup.h"
#include "board_constants.h"

void InterpretFEN(FEN_t fen, BoardInfo_t* info) {
    Square_t square = a8;

    int i = 0;
    while(fen[i] != '\0') {
        switch(fen[i])
        {
        case 'r':
            break;
        case 'R':
            /* code */
            break;

        default:
            break;
        }


        i++;
    }
}
