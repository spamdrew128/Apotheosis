#ifndef __ENGINE_TYPES_H__
#define __ENGINE_TYPES_H__

#include <stdint.h>
#include <stddef.h>

typedef int16_t EvalScore_t;
typedef int16_t Centipawns_t;

enum {
  INF = INT16_MAX - 1,
  EVAL_MAX = INF - 1,
  MATE_THRESHOLD = EVAL_MAX - 150,
};

typedef uint8_t Depth_t;
typedef uint8_t Ply_t;
typedef uint64_t NodeCount_t;
enum {
  PLY_MAX = 128,
  DEPTH_MAX = PLY_MAX - 40, // leaving room for qsearch to expand
};

typedef size_t Bytes_t;
typedef size_t Megabytes_t;
enum {
  bytes_per_megabyte = 1024 * 1024
};

#endif
