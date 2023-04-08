#ifndef __EVAL_CONSTANTS_TOOLS_H__
#define __EVAL_CONSTANTS_TOOLS_H__

#include <stdint.h>

#include "eval_constants.h"
#include "engine_types.h"

Bucket_t PSTBucketIndex(BoardInfo_t* boardInfo, Color_t color);

EvalScore_t EgFromScore(Score_t s);

EvalScore_t MgFromScore(Score_t s);

#endif