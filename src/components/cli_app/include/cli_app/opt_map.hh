#pragma once
#include <stdint.h>

typedef int8_t otokBaseT;

enum class otok : otokBaseT  {
  NONE = -1,  // not existing

  a, g, m, mm, c,


  SIZE // key array size
};

typedef const char *const_cstringT;

constexpr const_cstringT parmPair_keys[static_cast<otokBaseT>(otok::SIZE)] = {
    "a", "g", "m", "mm", "c",
};

otok optMap_findToken(const char *key);
