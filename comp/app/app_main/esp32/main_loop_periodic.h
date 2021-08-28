/**
 * \file   app_main/esp32/main_loop_periodic.h
 * \brief  Run events in main task by setting event bits from periodic 100ms timer
 */
#pragma once

#include "app_config/proj_app_cfg.h"

#include <utils_misc/int_types.h>
#include <utils_misc/int_macros.h>

extern uint32_t loop_flags_periodic_100ms;


inline void lfPer100ms_setBits(uint32_t v) {
  loop_flags_periodic_100ms |= (v);
}
inline void lfPer100ms_clrBits(uint32_t v) {
  loop_flags_periodic_100ms &= ~(v);
}

inline void lfPer100ms_setBit(loop_flagbits v) {
  lfPer100ms_setBits(BIT(v));
}
inline void lfPer100ms_clrBit(loop_flagbits v) {
  lfPer100ms_clrBits(BIT(v));
}
inline void lfPer100ms_putBit(loop_flagbits v, bool val) {
  if (val)
    lfPer100ms_setBit(v);
  else
    lfPer100ms_clrBit(v);
}

