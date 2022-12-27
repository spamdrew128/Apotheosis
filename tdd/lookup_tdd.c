#include <stdio.h>
#include <stdbool.h>

#include "lookup_tdd.h"
#include "lookup.h"
#include "board_constants.h"
#include "bitboards.h"

static void ShouldInitializeSingleBitset() {
    printf("\n%s Failure\n",__func__);
}

void LookupTDDRunner() {
    InitLookup();

    ShouldInitializeSingleBitset();
}