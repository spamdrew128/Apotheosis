#include "zobrist.h"
#include "RNG.h"
#include "bitboards.h"
#include "util_macros.h"

enum {
    white_queenside_castle_shift = c1,
    white_kingside_castle_shift  = g1 - 1,
    black_queenside_castle_shift = c8 - 2,
    black_kingside_castle_shift  = g8 - 3,
    castle_hash_bitmask = 0b1111
};

typedef uint64_t ZobristKey_t;

static ZobristKey_t whitePieceKeys[NUM_SQUARES * NUM_PIECES];
static ZobristKey_t blackPieceKeys[NUM_SQUARES * NUM_PIECES];
static ZobristKey_t castlingKeys[16];
static ZobristKey_t enPassantFileKeys[8];
static ZobristKey_t sideToMoveIsBlackKey;

static void FillKeysrandomList(ZobristKey_t* list, int num_entries) {
    for(int i = 0; i < num_entries; i++) {
        list[i] = RandUnsigned64();
    }
}

void GenerateZobristKeys() {
    FillKeysrandomList(whitePieceKeys, NUM_ARRAY_ELEMENTS(whitePieceKeys));
    FillKeysrandomList(blackPieceKeys, NUM_ARRAY_ELEMENTS(blackPieceKeys));
    FillKeysrandomList(castlingKeys, NUM_ARRAY_ELEMENTS(castlingKeys));
    FillKeysrandomList(enPassantFileKeys, NUM_ARRAY_ELEMENTS(enPassantFileKeys));
    sideToMoveIsBlackKey = RandUnsigned64();
}

void InitZobristStack(ZobristStack_t* zobristStack) {
    zobristStack->maxIndex = zobrist_stack_empty;
}

static void UpdateHashWithPieceBitboard(ZobristHash_t* zobristHash, Bitboard_t pieceBB, Piece_t type, ZobristKey_t* pieceKeys) {
    while(pieceBB) {
        Square_t square = LSB(pieceBB);
        *zobristHash ^= pieceKeys[NUM_PIECES*square + type];
        ResetLSB(&pieceBB);
    }
}

static void UpdateHashWithCastlingRights(ZobristHash_t* zobristHash, Bitboard_t whiteCastleRights, Bitboard_t blackCastleRights) {
    int keyIndex = castle_hash_bitmask & (
        (whiteCastleRights >> white_queenside_castle_shift) |
        (whiteCastleRights >> white_kingside_castle_shift) |
        (blackCastleRights >> black_queenside_castle_shift) |
        (blackCastleRights >> black_kingside_castle_shift));

    *zobristHash ^= castlingKeys[keyIndex];
}

static void UpdateHashEnPassantFile(ZobristHash_t* zobristHash, Bitboard_t enPassantBB) {
    *zobristHash ^= enPassantFileKeys[LSB(enPassantBB) % 8];
}

ZobristHash_t HashPosition(BoardInfo_t* boardInfo, GameStack_t* gameStack) {
    ZobristHash_t zobristHash = empty_set;
    GameState_t gameState = ReadCurrentGameState(gameStack);

    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->knights[white], knight, whitePieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->bishops[white], bishop, whitePieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->rooks[white], rook, whitePieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->queens[white], queen, whitePieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->pawns[white], pawn, whitePieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->kings[white], king, whitePieceKeys);

    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->knights[black], knight, blackPieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->bishops[black], bishop, blackPieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->rooks[black], rook, blackPieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->queens[black], queen, blackPieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->pawns[black], pawn, blackPieceKeys);
    UpdateHashWithPieceBitboard(&zobristHash, boardInfo->kings[black], king, blackPieceKeys);

    UpdateHashWithCastlingRights(&zobristHash, gameState.castleSquares[white], gameState.castleSquares[black]);

    if(gameState.canEastEP || gameState.canWestEP) {
        UpdateHashEnPassantFile(&zobristHash, gameState.enPassantSquare);
    }

    if(boardInfo->colorToMove == black) {
        zobristHash ^= sideToMoveIsBlackKey;
    }

    return zobristHash;
}

void AddZobristHashToStack(ZobristStack_t* zobristStack, ZobristHash_t hash) {
    zobristStack->maxIndex++;
    zobristStack->entries[zobristStack->maxIndex] = hash;
}

ZobristHash_t ZobristStackTop(ZobristStack_t* zobristStack) {
    return zobristStack->entries[zobristStack->maxIndex];
}

void RemoveZobristHashFromStack(ZobristStack_t* zobristStack) {
    zobristStack->maxIndex--;
}