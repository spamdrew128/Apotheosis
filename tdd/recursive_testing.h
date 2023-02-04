#ifndef __RECURSIVE_TESTING_H__
#define __RECURSIVE_TESTING_H__

#include <stdbool.h>

void UnmakeRecursiveTestRunner(bool runTests, int depth, FEN_t fen);

void PERFTRunner(bool runTests, int depth, FEN_t fen);

#endif