/**
 * \file   app_main/esp32/main_loop_periodic.h
 * \brief  Run events in main task by setting event bits from periodic 100ms timer
 */
#pragma once
#include <stdbool.h>


/**
 * \brief  Flags to turn on/off a periodic event
 */
union lfPerFlags {
  struct {
#ifdef CONFIG_APP_USE_SEP
    bool lf_loopFerSep :1; ///< enable set-end-position loop
#endif
#ifdef CONFIG_APP_USE_CUAS
    bool lf_checkCuasTimeout : 1; ///< enable cu-auto-scan timeout
#endif
#ifdef CONFIG_APP_USE_PAIRINGS
    bool lf_checkPairingTimeout :1; ///< enable check pairing timeout
#endif
    bool lf_loopPosAutoSave :1;  ///< enable automatic shutter position save
  } flags;
  unsigned flagbits;  ///< all flags as bit representation for convenience
};

extern union lfPerFlags mainLoop_PeriodicFlags; ///< enable/disable conditional periodic events

/**
 * \brief Called every 100ms in main thread context
 */
void lfPer100ms_mainFun();

/**
 * \brief Start timer for periodic events. Will also restart MCU periodically to avoid memory fragmentation.
 */
void tmr_loopPeriodic100ms_start();
