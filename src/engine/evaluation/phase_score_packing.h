#ifndef __PHASE_SCORE_PACKING_H__
#define __PHASE_SCORE_PACKING_H__

#include <stdint.h>
#include "engine_types.h"

typedef int32_t Score_t;

#define S(mg, eg) \
	((Score_t)((uint32_t)eg << 16) + mg)

inline EvalScore_t EgFromScore(Score s) {
	union { uint16_t u; int16_t s; } eg = { (uint16_t)((uint32_t)(s + 0x8000) >> 16) };
	return eg.s;
}

inline EvalScore_t MgFromScore(Score s) {
	union { uint16_t u; int16_t s; } mg = { (uint16_t)(uint32_t)s };
	return mg.s;
}

#endif