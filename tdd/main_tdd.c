#include <stdio.h>

#include "board_constants.h"
#include "lookup_tdd.h"
#include "bitboards_tdd.h"
#include "FEN_tdd.h"

int main(int argc, char *argv[])
{
    LookupTDDRunner();
    BitboardsTDDRunner();
    FENTDDRunner();
}