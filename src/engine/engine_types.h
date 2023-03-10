#ifndef __ENGINE_TYPES_H__
#define __ENGINE_TYPES_H__

#include <stdint.h>
#include <stddef.h>

typedef int16_t EvalScore_t;
typedef int16_t Centipawns_t;

enum {
  EVAL_MAX = 30000,
  INFINITY = INT16_MAX - 1
};

typedef uint8_t Depth_t;
typedef uint8_t Ply_t;
typedef uint64_t NodeCount_t;
enum {
  PLY_MAX = 128
};

typedef size_t Bytes_t;
typedef size_t Megabytes_t;
enum {
  bytes_per_megabyte = 1024 * 1024
};

#endif
