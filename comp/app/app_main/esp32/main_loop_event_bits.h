/**
 * \file   app_main/esp32/main_loop_event_bits.h
 * \brief  Run events in main task by setting event bits from other tasks
 */

#pragma once

#include "app_config/proj_app_cfg.h"

#include <utils_misc/int_types.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <esp_event.h>
#include <esp_system.h>

extern EventGroupHandle_t loop_event_group;

inline void lf_setBits(const EventBits_t uxBitsToSet) {
  xEventGroupSetBits(loop_event_group, uxBitsToSet);
}
void lf_setBits_ISR(const EventBits_t uxBitsToSet, bool yield);

extern volatile uint32_t loop_flags;

inline void lf_setBit(loop_flagbits v) {
  lf_setBits(BIT(v));
}
inline void IRAM_ATTR lf_setBit_ISR(loop_flagbits v, bool yield) {
  lf_setBits_ISR(BIT(v), yield);
}

void loop_eventBits_setup();
u32 loop_eventBits_wait();
void loop_eventBits_check();
