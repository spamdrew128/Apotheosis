#include "zobrist.h"
#include "RNG.h"

#define NUM_ARRAY_ELEMENTS(array) sizeof(array) / sizeof(*array)

typedef uint64_t ZobristPsuedorandom_t;

static ZobristPsuedorandom_t whitePiecePsuedos[NUM_SQUARES * NUM_PIECES];
static ZobristPsuedorandom_t blackPiecePsuedos[NUM_SQUARES * NUM_PIECES];
static ZobristPsuedorandom_t castlingPsuedos[16];
static ZobristPsuedorandom_t enPassantFilePsuedos[8];
static ZobristPsuedorandom_t sideToMoveIsBlackPsuedo;

static void FillPsuedorandomList(ZobristPsuedorandom_t* list, int num_entries) {
    for(int i = 0; i < num_entries; i++) {
        list[i] = RandBitboard();
    }
}

void InitZobristGenerator() {
    FillPsuedorandomList(whitePiecePsuedos, NUM_ARRAY_ELEMENTS(whitePiecePsuedos));
    FillPsuedorandomList(blackPiecePsuedos, NUM_ARRAY_ELEMENTS(blackPiecePsuedos));
    FillPsuedorandomList(castlingPsuedos, NUM_ARRAY_ELEMENTS(castlingPsuedos));
    FillPsuedorandomList(enPassantFilePsuedos, NUM_ARRAY_ELEMENTS(enPassantFilePsuedos));
    sideToMoveIsBlackPsuedo = RandBitboard();
}

void InitZobristStack(ZobristStack_t* zobristStack) {
    zobristStack->num_entries = 0;
}