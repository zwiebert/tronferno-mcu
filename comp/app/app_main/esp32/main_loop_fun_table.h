#pragma once

#include "app_config/proj_app_cfg.h"

enum loop_flagbits {
  lf_mainLoopQueue,
#if defined USE_AP_FALLBACK || defined USE_WLAN_AP
  lf_createWifiAp,
#endif
  lf_loopCli,
#ifdef USE_SEP
  lf_loopFerSep,
#endif
  lf_loopFerPos,
  lf_loopFerTimerState,
  lf_loopFauTimerDataHasChanged,
#ifdef USE_CUAS
  lf_checkCuasTimeout,
#endif
#ifdef USE_PAIRINGS
  lf_checkPairingTimeout,
#endif
  lf_loopPosAutoSave, lf_loopPosCheckMoving,
  lf_gpio_input_intr,


//---------------
  lf_Len
};

void loop_fun_table_call(loop_flagbits fb);
void loop_fun_table_calls(uint32_t bitmask);




