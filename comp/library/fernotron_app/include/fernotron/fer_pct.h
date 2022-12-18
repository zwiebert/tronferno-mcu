/**
 * \file    fernotron/fer_pct.h
 * \brief   class Pct to represent shutter opening percentage
 */


#pragma once

#ifdef __cplusplus
#include "fernotron/gm_set.hh"
#endif


#include <stdint.h>
#include "stdbool.h"
#include "utils_misc/int_macros.h"

struct Pct {
  uint8_t pct_ = 0;
  bool invalid_ = false;
public:
  uint8_t value() const { return pct_; }
  operator bool () const { return !invalid_; };
  operator uint8_t () const { return pct_; }
  Pct& operator =(uint8_t pct) { pct_ = pct; return *this; }
  bool operator ==(const Pct &other) const {  return pct_ == other.pct_;  }
  bool operator<(const Pct &other) const { return pct_ < other.pct_; }
  Pct(uint8_t pct) : pct_(pct) {}
  Pct() : invalid_(true) { }
};
