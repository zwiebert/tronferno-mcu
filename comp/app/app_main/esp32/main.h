#pragma once

#include "app_config/proj_app_cfg.h"

#include <utils_misc/int_types.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <esp_event.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_sntp.h>
#include <string.h>

#define KEY_BOOT_COUNTER "BOOT_CT"
extern EventGroupHandle_t loop_event_group;
#define lf_setBits(v) xEventGroupSetBits(loop_event_group, static_cast<EventBits_t>(v))
void lf_setBits_ISR(const EventBits_t uxBitsToSet, bool yield);


extern volatile uint32_t loop_flags;
extern uint32_t loop_flags_periodic;
extern i32 boot_counter;
extern bool wifi_ap_active;


enum loop_flagbits {
#ifdef USE_NETWORK
  lf_gotIpAddr, lf_lostIpAddr,
#endif
#ifdef USE_NTP
  lf_ntpSyncTime,
#endif
#if defined USE_AP_FALLBACK || defined USE_WLAN_AP
  lf_createWifiAp,
#endif
#ifdef USE_TCPS
  lf_loopTcpServer,
#endif
  lf_loopCli,
#ifdef FER_TRANSMITTER
  lf_loopFerTx,
#endif
#ifdef FER_RECEIVER
  lf_loopFerRx,
#endif
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
  lf_mcuRestart,

  //-------------
  lf_Len
};


inline void lf_setBit(loop_flagbits v) {
  lf_setBits(1 << static_cast<unsigned>(v));
}
inline void IRAM_ATTR lf_setBit_ISR(loop_flagbits v, bool yield) {
  lf_setBits_ISR((1 << static_cast<unsigned>(v)), yield);
}

#define lfPer_setBits(v) (loop_flags_periodic |= (v))
#define lfPer_clrBits(v) (loop_flags_periodic &= ~(v))

inline void lfPer_setBit(loop_flagbits v) {
  lfPer_setBits(1 << (v));
}
inline void lfPer_clrBit(loop_flagbits v) {
  lfPer_clrBits(1 << (v));
}
inline void lfPer_putBit(loop_flagbits v, bool val) {
  if (val)
    lfPer_setBit(v);
  else
    lfPer_clrBit(v);
}



void lfa_createWifiAp(void);

void loop(void);
void tmr_checkNetwork_start();
void tmr_loopPeriodic_start();
void tmr_pingLoop_start();
void loop_eventBits_setup();
u32 loop_eventBits_wait();

void  mcu_delayedRestart(unsigned delay_ms);

extern "C++" void main_setup_ip_dependent();
void mcu_init();


#define D(x) x

