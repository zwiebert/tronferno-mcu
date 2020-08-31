#pragma once

#include "app_config/proj_app_cfg.h"

#include "app/common.h"
#include "app/timer.h"
#include "cli_app/cli_app.h"
#include "config/config.h"
#include "driver/gpio.h"
#include "fernotron/callbacks.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/fer_msg_tx.h"
#include "fernotron/fer_radio_trx.h"
#include "fernotron_alias/pairings.h"
#include "fernotron_auto/fau_tevent.h"
#include "fernotron_cuas/cuid_auto_set.h"
#include "fernotron_sep/set_endpos.h"
#include "fernotron_txtio/fer_print.h"
#include "fernotron_pos/shutter_pct.h"
#include "gpio/pin.h"
#include "key_value_store/kvs_wrapper.h"
#include "misc/int_macros.h"
#include "misc/int_types.h"
#include "net/ethernet.h"
#include "net/ipnet.h"
#include "net/mqtt/app/mqtt.h"
#include "net/ntp.h"
#include "net/tcp_cli_server.h"
#include "net/wifi_ap.h"
#include "net/wifistation.h"
#include "nvs_flash.h"
#include "storage/storage.h"
#include "string.h"
#include "txtio/inout.h"

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
#ifdef USE_EG
extern EventGroupHandle_t loop_event_group;
#define lf_setBits(v) xEventGroupSetBits(loop_event_group, (v))
void lf_setBits_ISR(const EventBits_t uxBitsToSet, bool yield);
#else
#define lf_setBits(v) (loop_flags |= (v))
#define lf_setBits_ISR(v,y) lf_setBits((v))
#endif

#define lf_setBit(v)  lf_setBits(1<<(v))
#define lf_setBit_ISR(v, yield)  lf_setBits_ISR((1<<(v)), yield)
#define lfPer_setBits(v) (loop_flags_periodic |= (v))
#define lfPer_clrBits(v) (loop_flags_periodic &= ~(v))
#define lfPer_setBit(v)  lfPer_setBits(1<<(v))
#define lfPer_clrBit(v)  lfPer_clrBits(1<<(v))


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


extern volatile uint32_t loop_flags;
extern uint32_t loop_flags_periodic;
extern i32 boot_counter;
extern bool wifi_ap_active;
extern SemaphoreHandle_t uart_mutex;

void lfa_createWifiAp(void);
void lfa_gotIpAddr(void);
void lfa_lostIpAddr(void);
void lfa_ntpSync(void);
void lfa_mcuRestart(void);

void loop(void);
void tmr_checkNetwork_start();
void tmr_loopPeriodic_start();
void tmr_pingLoop_start();
void loop_eventBits_setup();
u32 loop_eventBits_wait();

extern "C" void main_setup_ip_dependent();
void mcu_init();


#define D(x) x

