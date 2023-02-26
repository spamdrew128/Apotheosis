#ifndef __RECURSIVE_TESTING_H__
#define __RECURSIVE_TESTING_H__

#include <stdbool.h>
#include "FEN.h"

void UnmakeRecursiveTestRunner(FEN_t fen, int depth, bool runTests);

void PERFTRunner(FEN_t fen, int depth, bool runTests);

void SpeedTest(FEN_t fen, int depth, bool runTests);

#endif