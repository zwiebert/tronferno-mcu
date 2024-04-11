#pragma once

#include "main_loop_periodic.h"

#include <utils_misc/int_types.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"

#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_sntp.h>

#include <string.h>

#define KEY_BOOT_COUNTER "BOOT_CT"

extern int32_t boot_counter;
extern bool wifi_ap_active;

void lfa_createWifiAp(void);
void lfa_removeWifiAp_if_never_used(); ///<  Remove AP if there were no logins since the AP had started.
void lfa_removeWifiAp_if_unused(); ///<  Remove AP if there is currently no user logged in.

void loop(void);
void nwc_tryFallback_later();
void tmr_loopPeriodic100ms_start();
void tmr_pingLoop_start();
void loop_eventBits_setup();
uint32_t loop_eventBits_wait();

void mcu_delayedRestart(unsigned delay_ms);

extern "C++" void main_setup_ip_dependent();
void mcu_init();

