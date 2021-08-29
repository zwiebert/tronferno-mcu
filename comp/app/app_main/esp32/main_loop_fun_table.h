#pragma once

#include "app_config/proj_app_cfg.h"

enum loop_flagbits {
  lf_mainLoopQueue,
#ifdef USE_SEP
  lf_loopFerSep,
#endif
  lf_loopFerPos,
  lf_loopFerTimerState,
#ifdef USE_CUAS
  lf_checkCuasTimeout,
#endif
#ifdef USE_PAIRINGS
  lf_checkPairingTimeout,
#endif
  lf_loopPosAutoSave, lf_loopPosCheckMoving,

//---------------
  lf_Len
};

void loop_fun_table_call(loop_flagbits fb);
void loop_fun_table_calls(uint32_t bitmask);




