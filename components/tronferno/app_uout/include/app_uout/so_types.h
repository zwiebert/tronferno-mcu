#pragma once

#include <uout/uo_types.h>
#include <stdint.h>

typedef struct {
  const char *key, *val;
} so_arg_kvs_t;

typedef struct {
  const char *key;
  int val;
} so_arg_kvd_t;

