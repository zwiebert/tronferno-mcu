/*
 * cuid_auto_scan.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include <string.h>



#include "fernotron/cuas/cuid_auto_set.h"

#include "fernotron/fer_main.h"
#include "utils_misc/int_types.h"
#include "fernotron_uout/fer_uo_publish.h"
#include <utils_time/run_time.h>
#include "fernotron_trx/fer_trx_c_api.h"

#include <stdint.h>

void (*fer_cuas_enable_disable_cb)(bool enable, uint32_t cu);

static inline void fer_cuas_ENABLE_cb() {
  if (fer_cuas_enable_disable_cb)
    fer_cuas_enable_disable_cb(true, 0);
}
static inline void fer_cuas_DISABLE_cb(uint32_t cu = 0) {
  if (fer_cuas_enable_disable_cb)
    fer_cuas_enable_disable_cb(false, cu);
}

static fer_cuas_state_T fer_cuas_state;

fer_cuas_state_T fer_cuas_getState() {
  return fer_cuas_state;
}

static bool fer_cuas_active;
static time_t end_time;

bool fer_cuas_set(uint16_t id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;

  if (timeout_secs > 0) {
    end_time = run_time_s() + timeout_secs;
    fer_cuas_active = true;
    fer_cuas_state = FER_CUAS_SCANNING;
    uoApp_publish_fer_cuasState({.scanning = true});
    fer_cuas_ENABLE_cb();
  }
  return false;
}

void fer_cuas_set_check_timeout() {
  if (end_time == 0)
    return;

  if (end_time < run_time_s()) {
    end_time = 0;
    uoApp_publish_fer_cuasState({.timeout = true});
    fer_cuas_state = FER_CUAS_TIME_OUT;
    fer_cuas_active = false;
    fer_cuas_DISABLE_cb();
  }
}

bool fer_cuas_set_check(const uint32_t cu) {
  if (end_time == 0)
    return false;

  end_time = 0;
  uoApp_publish_fer_cuasState( { .a = cu, .success = true });
  fer_cuas_state = FER_CUAS_SUCCESS;

  fer_cuas_active = false;
  fer_cuas_DISABLE_cb(cu);
  return true;
}

