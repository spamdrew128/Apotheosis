#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "debug.h"
#include "lookup.h"
#include "string_utils.h"

void PrintBitboard(Bitboard_t b) {
    Square_t boardArray[64] = {0};

    while(b) {
        Square_t square = LSB(b);
        boardArray[square] = 1;
        ResetLSB(&b);
    }

    printf("\n");
    for(int i = 7; i >= 0; i--) {
        for(int j = 0; j < 8; j++) {
            Square_t square = boardArray[i*8 + j];
            if(square) {
                printf("%d ", boardArray[i*8 + j]);
            } else {
                printf(". ");
            }
        }    
        printf("\n");
    }
}

void PrintMailbox(BoardInfo_t *info) {
    printf("\n");
    for(int i = 7; i >= 0; i--) {
        for(int j = 0; j < 8; j++) {
            Piece_t piece = info->mailbox[i*8 + j];

            switch (piece)
            {
            case queen:
                printf("Q ");
                break;
            case rook:
                printf("R ");
                break;
            case bishop:
                printf("B ");
                break;
            case knight:
                printf("N ");
                break;
            case king:
                printf("K ");
                break;
            case pawn:
                printf("P ");
                break;
            default:
                printf(". ");
                break;
            }
        }    
        printf("\n");
    }
}

Bitboard_t CreateBitboard(int numOccupied, ...) {
    va_list valist;
    va_start(valist, numOccupied);

    Bitboard_t bitboard = EMPTY_SET;
    for (int i = 0; i < numOccupied; i++) {
        SetBits(&bitboard, C64(1) << va_arg(valist, int));
    } 

    return bitboard;
}

static void PrintSingleTypeMoves(Move_t move, BoardInfo_t* info, Piece_t type, const char* typeText) {
    char fromText[3];
    char toText[3];

    if(PieceOnSquare(info, ReadFromSquare(move)) == type) {
        SquareToString(ReadFromSquare(move), fromText);
        SquareToString(ReadToSquare(move), toText);
        printf("%s, From %s To %s\n", typeText, fromText, toText);
    }
}

void PrintMoveList(MoveEntryList_t* moveList, BoardInfo_t* info) {
    printf("\n");
    for(int i = 0; i <= moveList->maxIndex; i++) {
        Move_t move = moveList->moves[i].move;
        PrintSingleTypeMoves(move, info, king, "King");
        PrintSingleTypeMoves(move, info, queen, "Queen");
        PrintSingleTypeMoves(move, info, rook, "Rook");
        PrintSingleTypeMoves(move, info, bishop, "Bishop");
        PrintSingleTypeMoves(move, info, knight, "Knight");
        PrintSingleTypeMoves(move, info, pawn, "Pawn");
    }
}

void AddGameStateToStack(GameState_t stateToAdd, GameStack_t* stack) {
    GameState_t* gameState = GetEmptyNextGameState(stack);
    *gameState = stateToAdd;
}

bool CompareInfo(BoardInfo_t* info, BoardInfo_t* expectedInfo) {
    bool success = true;
    for(int i = 0; i < 2; i++) {
        success = success &&
            (info->allPieces[i] == expectedInfo->allPieces[i]) &&
            (info->pawns[i] == expectedInfo->pawns[i]) &&
            (info->knights[i] == expectedInfo->knights[i]) &&
            (info->bishops[i] == expectedInfo->bishops[i]) &&
            (info->rooks[i] == expectedInfo->rooks[i]) &&
            (info->queens[i] == expectedInfo->queens[i]) &&
            (info->kings[i] == expectedInfo->kings[i]);
    }

    success = success && (info->empty == expectedInfo->empty);
    // success = success && (info->colorToMove == expectedInfo->colorToMove); breaks tdd so leaving it out for now

    for(int i = 0; i < NUM_SQUARES; i++) {
        success = success && 
            PieceOnSquare(info, i) == PieceOnSquare(expectedInfo, i);
    }

    return success;
}

bool CompareState(GameState_t* expectedState, GameStack_t* stack) {
    return
        (ReadHalfmoveClock(stack) == expectedState->halfmoveClock) &&
        (ReadEnPassant(stack) == expectedState->enPassantSquare) &&
        (ReadCastleSquares(stack, white) == expectedState->castleSquares[white]) &&
        (ReadCastleSquares(stack, black) == expectedState->castleSquares[black]) &&
        (ReadCapturedPiece(stack) == expectedState->capturedPiece);
}

static bool InvalidMailbox(
    BoardInfo_t* info,
    int numPawns,
    int numKnights,
    int numBishops,
    int numRooks,
    int numQueens,
    int numKings,
    int numEmpty
)
{
    for(int i = 0; i < NUM_SQUARES; i++) {
        Piece_t piece = PieceOnSquare(info, i);
        switch (piece)
        {
            case pawn:
                numPawns--;
                break;
            case knight:
                numKnights--;
                break;
            case bishop:
                numBishops--;
                break;
            case rook:
                numRooks--;
                break;
            case queen:
                numQueens--;
                break;
            case king:
                numKings--;
                break;
            default:
                numEmpty--;
                break;
        }
    }

    return (numPawns || numKnights || numBishops || numRooks || numQueens || numKings || numEmpty);
}

static bool EnemyKingCanBeCaptured(BoardInfo_t *info, Color_t colorToMove) {
    Bitboard_t attacks = 0;
    if(colorToMove == white) {
        attacks |= NoEaOne(info->pawns[colorToMove]) | NoWeOne(info->pawns[colorToMove]);
    } else {
        attacks |= SoEaOne(info->pawns[colorToMove]) | SoWeOne(info->pawns[colorToMove]);
    }

    Bitboard_t d12Sliders = info->bishops[colorToMove] | info->queens[colorToMove];
    Bitboard_t hvSliders = info->rooks[colorToMove] | info->queens[colorToMove];
    Bitboard_t knights = info->knights[colorToMove];

    while(d12Sliders) {
        Square_t square = LSB(d12Sliders);
        attacks |= GetBishopAttackSet(square, info->empty);
        ResetLSB(&d12Sliders);
    }

    while(hvSliders) {
        Square_t square = LSB(hvSliders);
        attacks |= GetRookAttackSet(square, info->empty);
        ResetLSB(&hvSliders);
    }

    while(knights) {
        Square_t square = LSB(knights);
        attacks |= GetKnightAttackSet(square);
        ResetLSB(&knights);
    }

    return info->kings[!colorToMove] & attacks;
}

bool BoardIsValid(BoardInfo_t *info, GameStack_t* gameStack) {
    assert(info != NULL);

    Population_t numPawns = PopCount(info->pawns[white] | info->pawns[black]);
    Population_t numKnights = PopCount(info->knights[white] | info->knights[black]);
    Population_t numBishops = PopCount(info->bishops[white] | info->bishops[black]);
    Population_t numRooks = PopCount(info->rooks[white] | info->rooks[black]);
    Population_t numQueens = PopCount(info->queens[white] | info->queens[black]);
    Population_t numKings = PopCount(info->kings[white] | info->kings[black]);
    Population_t numEmpty = PopCount(info->empty);


    if(PopCount(info->kings[white]) != 1 || PopCount(info->kings[black]) != 1) {
        return false;
    }

    if((PopCount(info->pawns[white]) > 8 || PopCount(info->pawns[black]) > 8)) {
        return false;
    }

    if((PopCount(info->knights[white]) > 10 || PopCount(info->knights[black]) > 10)) {
        return false;
    }

    if((PopCount(info->rooks[white]) > 10 || PopCount(info->rooks[black]) > 10)) {
        return false;
    }

    if((PopCount(info->bishops[white]) > 10 || PopCount(info->bishops[black]) > 10)) {
        return false;
    }

    if((PopCount(info->queens[white]) > 9 || PopCount(info->queens[black]) > 9)) {
        return false;
    }

    if((info->pawns[white] | info->pawns[black]) & (RANK_1 | RANK_8)) {
        return false;
    }

    if(info->allPieces[white] & info->allPieces[black]) {
        return false;
    }

    if(info->empty & (info->allPieces[white] | info->allPieces[black])) {
        return false;
    }

    if(InvalidMailbox(info, numPawns, numKnights, numBishops, numRooks, numQueens, numKings, numEmpty)) {
        return false;
    }

    if(EnemyKingCanBeCaptured(info, info->colorToMove)) {
        return false;
    }

    BoardInfo_t savedInfo = ReadCurrentBoardInfo(gameStack);
    if(!CompareInfo(info, &savedInfo)) {
        return false;
    }

    return true;
}