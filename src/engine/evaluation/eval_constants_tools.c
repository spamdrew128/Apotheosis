#include <stdbool.h>

#include "eval_constants_tools.h"

Bucket_t PSTBucketIndex(BoardInfo_t* boardInfo, Color_t color) {
    // indexed by opponent king position
    // qs is index 0, ks is index 1
    return (bool)(boardInfo->kings[!color] & KS_PST_MASK);
}

EvalScore_t EgFromScore(Score_t s) {
    union { uint16_t u; int16_t s; } eg = { (uint16_t)((uint32_t)(s + 0x8000) >> 16) };
    return eg.s;
}

EvalScore_t MgFromScore(Score_t s) {
    union { uint16_t u; int16_t s; } mg = { (uint16_t)(uint32_t)s };
    return mg.s;
}