#pragma once

#include <fernotron/fsb.h>
#include <stdint.h>
#include <fernotron/gm_bitmask.hh>

struct fer_configT {
  uint32_t cu;
};

extern fer_sbT default_sender;
extern  Fer_GmBitMask manual_bits;

void fer_main_setup(const fer_configT &ferConfig, bool reinit = false);
fer_sbT *fer_main_getSenderByAddress(long addr);
