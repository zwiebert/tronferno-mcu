#pragma once

#include <stdint.h>
#include <fernotron/gm_bitmask.hh>

struct fer_configT {
  uint32_t cu;
};

extern  Fer_GmBitMask manual_bits;

void fer_main_setup(const fer_configT &ferConfig, bool reinit = false);
uint32_t fer_main_getSenderByAddress(long addr);
