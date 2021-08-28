/**
 * \file   app_main/esp32/main_loop_event_bits.h
 * \brief  Run events in main task by setting event bits from other tasks
 */

#pragma once

#include "app_config/proj_app_cfg.h"
#include <utils_misc/int_types.h>
#include <utils_misc/int_macros.h>

/**
 * \brief  Start events in main task by setting the related flags from same or other task
 * \param uxBitsToSet  Event bits according to \ref loop_flagbits
 */
void lf_setBits(uint32_t uxBitsToSet);

/**
 * \brief  Start events in main task by setting the related flags from ISR
 * \param uxBitsToSet  Event bits according to \ref loop_flagbits
 */
void lf_setBits_ISR(uint32_t uxBitsToSet, bool yield);

/**
 * \brief  Start event in main task by setting the related flag from same or other task
 * \param v  Event bit according to \ref loop_flagbits
 */
inline void lf_setBit(loop_flagbits v) {
 lf_setBits(BIT(v));
}

/**
 * \brief  Start event in main task by setting the related flag from ISR
 * \param v  Event bit according to \ref loop_flagbits
 */
inline void IRAM_ATTR lf_setBit_ISR(loop_flagbits v, bool yield) {
 lf_setBits_ISR(BIT(v), yield);
}

void loop_eventBits_setup();
u32 loop_eventBits_wait();
void loop_eventBits_check();
