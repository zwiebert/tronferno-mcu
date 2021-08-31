/**
 * \file   app_main/esp32/main_loop_periodic.h
 * \brief  Run events in main task by setting event bits from periodic 100ms timer
 */
#pragma once

#include "app_config/proj_app_cfg.h"

#include <utils_misc/int_types.h>
#include <utils_misc/int_macros.h>

union lfPerFlags {
  struct {
#ifdef USE_SEP
    bool lf_loopFerSep :1;
#endif
    bool lf_loopFerPos :1;
    bool lf_loopFerTimerState :1;
#ifdef USE_CUAS
    bool lf_checkCuasTimeout : 1;
#endif
#ifdef USE_PAIRINGS
    bool lf_checkPairingTimeout :1;
#endif
    bool lf_loopPosAutoSave :1;
    bool lf_loopPosCheckMoving :1;
  } flags;
  unsigned bitmask;
};

extern lfPerFlags mainLoop_PeriodicFlags;

void lfPer100ms_mainFun();
