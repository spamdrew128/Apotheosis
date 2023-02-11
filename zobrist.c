#include "zobrist.h"
#include "RNG.h"

typedef uint64_t ZobristPsuedorandom_t;

static ZobristPsuedorandom_t whitePiecePsuedos[NUM_SQUARES * NUM_PIECES];
static ZobristPsuedorandom_t blackPiecePsuedos[NUM_SQUARES * NUM_PIECES];
static ZobristPsuedorandom_t castlingPsuedos[16];
static ZobristPsuedorandom_t enPassantFilePsuedos[8];
static ZobristPsuedorandom_t sideToMoveIsBlackPsuedo;

