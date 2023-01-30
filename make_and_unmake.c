#include "make_and_unmake.h"

static void CastlingMakeHandler(BoardInfo_t* boardInfo, Move_t move, Color_t color) {
    Square_t fromSquare = ReadFromSquare(move);
    Square_t toSquare = ReadToSquare(move);

    if(toSquare < fromSquare) { // queenside castle
        boardInfo->kings[color] 
    }
}

void MakeMove(BoardInfo_t* boardInfo, Move_t move, Color_t color) {
    SpecialFlag_t specialFlag = ReadSpecialFlag(move);
    
    switch (specialFlag) {
        case castle_flag:
            CastlingMakeHandler(boardInfo, move, color);
        break;
        case promotion_flag:
        /* code */
        break;
        case en_passant_flag:
        /* code */
        break;
        default:
        break;
    }
}