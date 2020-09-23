/*
 * cuid_auto_scan.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include <string.h>
#include "app/config/proj_app_cfg.h"

#include "fernotron/cuas/cuid_auto_set.h"

#include "app/settings/config.h"
#include "app/common.h"
#include "app/uout/status_output.h"
#include "app/rtc.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/fer_msg_attachment.h"

static const struct TargetDesc *my_td;

void (*fer_cuas_enable_disable_cb)(bool enable);

static inline void fer_cuas_ENABLE_cb() {
  if (fer_cuas_enable_disable_cb)
    fer_cuas_enable_disable_cb(true);
}
static inline void fer_cuas_DISABLE_cb() {
  if (fer_cuas_enable_disable_cb)
    fer_cuas_enable_disable_cb(false);
}

static fer_cuas_state_T fer_cuas_state;

fer_cuas_state_T fer_cuas_getState() {
  return fer_cuas_state;
}

static bool fer_cuas_active;
static time_t end_time;

bool fer_cuas_set(const struct TargetDesc &td, u16 id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;
  my_td = &td;

  if (timeout_secs > 0) {
    end_time = run_time_s() + timeout_secs;
    last_received_sender.data[0] = 0;
    fer_cuas_active = true;
    soMsg_cuas_start(*my_td, id);
    fer_cuas_state = FER_CUAS_SCANNING;
    fer_cuas_ENABLE_cb();
  }
  return false;
}

void fer_cuas_set_check_timeout() {
  if (end_time == 0)
    return;

  if (end_time < run_time_s()) {
    end_time = 0;
    soMsg_cuas_timeout(*my_td);
    fer_cuas_state = FER_CUAS_TIME_OUT;
    fer_cuas_active = false;
    fer_cuas_DISABLE_cb();
  }
}
#define CI(cb) static_cast<configItem>(cb)

bool fer_cuas_set_check(const fer_sbT *fsb) {
  if (end_time == 0)
    return false;

  if (FER_SB_ADDR_IS_CENTRAL(fsb)) {
    u32 cu = FER_SB_GET_DEVID(fsb);
    config_save_item_n_u32(CI(CB_CUID), cu);
    config_item_modified(CI(CB_CUID));
    end_time = 0;
    soMsg_cuas_done(*my_td);
    fer_cuas_state = FER_CUAS_SUCCESS;
    config_save_item_n_u32(CI(CB_CUID), cu);
    config_item_modified(CI(CB_CUID));
    fer_cuas_active = false;
    fer_cuas_DISABLE_cb();
    return true;
  }

  return false;
}
