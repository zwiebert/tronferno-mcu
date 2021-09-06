/**
 * \file  fernotron/fer_main.h
 * \brief Component settings.
 */
#pragma once

#include <stdint.h>
#include <fernotron/gm_set.hh>

struct fer_configT {
  uint32_t cu;
  uint32_t usedMembers;
};

extern fer_configT fer_config;
extern  Fer_GmSet manual_bits;
#define MANUAL_BITS_STORE_NAME "GMBM_MANU"
extern  Fer_GmSet fer_usedMemberMask;

void fer_main_setup(const fer_configT &ferConfig, bool reinit = false);
uint32_t fer_main_getSenderByAddress(long addr);
