#include "zobrist.h"
#include "RNG.h"

#define NUM_ARRAY_ELEMENTS(array) sizeof(array) / sizeof(*array)

typedef uint64_t ZobristKey_t;

static ZobristKey_t whitePiecePsuedos[NUM_SQUARES * NUM_PIECES];
static ZobristKey_t blackPiecePsuedos[NUM_SQUARES * NUM_PIECES];
static ZobristKey_t castlingPsuedos[16];
static ZobristKey_t enPassantFilePsuedos[8];
static ZobristKey_t sideToMoveIsBlackPsuedo;

static void FillPsuedorandomList(ZobristKey_t* list, int num_entries) {
    for(int i = 0; i < num_entries; i++) {
        list[i] = RandUnsigned64();
    }
}

void InitZobristGenerator() {
    FillPsuedorandomList(whitePiecePsuedos, NUM_ARRAY_ELEMENTS(whitePiecePsuedos));
    FillPsuedorandomList(blackPiecePsuedos, NUM_ARRAY_ELEMENTS(blackPiecePsuedos));
    FillPsuedorandomList(castlingPsuedos, NUM_ARRAY_ELEMENTS(castlingPsuedos));
    FillPsuedorandomList(enPassantFilePsuedos, NUM_ARRAY_ELEMENTS(enPassantFilePsuedos));
    sideToMoveIsBlackPsuedo = RandUnsigned64();
}

void InitZobristStack(ZobristStack_t* zobristStack) {
    zobristStack->num_entries = 0;
}