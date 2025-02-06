#pragma once


#include <stdint.h>

#define KEY_BOOT_COUNTER "BOOT_CT"

extern int32_t boot_counter;
extern bool wifi_ap_active;

void loop(void);
void nwc_tryFallback_later();
void tmr_loopPeriodic100ms_start();
void tmr_pingLoop_start();
void loop_eventBits_setup();
uint32_t loop_eventBits_wait();

void mcu_delayedRestart(unsigned delay_ms);

void main_setup_ip_dependent();
void mcu_init();

