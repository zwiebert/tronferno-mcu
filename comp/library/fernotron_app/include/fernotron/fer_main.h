#pragma once

#include <fernotron/fsb.h>
#include <stdint.h>
#include <fernotron/gm_bitmask.hh>

struct fer_configT {
  uint32_t cu;
};

extern fer_sbT default_sender;
extern  GmBitMask manual_bits;

void fer_setup(const fer_configT &ferConfig, bool reinit = false);
fer_sbT *get_sender_by_addr(long addr);
