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

void (*cuas_enable_disable_cb)(bool enable);

static inline void cuas_ENABLE_cb() {
  if (cuas_enable_disable_cb)
    cuas_enable_disable_cb(true);
}
static inline void cuas_DISABLE_cb() {
  if (cuas_enable_disable_cb)
    cuas_enable_disable_cb(false);
}

static cuas_state_T cuas_state;

cuas_state_T cuas_getState() {
  return cuas_state;
}

static bool cuas_active;
static time_t end_time;

bool cu_auto_set(const struct TargetDesc &td, u16 id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;
  my_td = &td;

  if (timeout_secs > 0) {
    end_time = run_time_s() + timeout_secs;
    last_received_sender.data[0] = 0;
    cuas_active = true;
    soMsg_cuas_start(*my_td, id);
    cuas_state = CUAS_SCANNING;
    cuas_ENABLE_cb();
  }
  return false;
}

void cu_auto_set_check_timeout() {
  if (end_time == 0)
    return;

  if (end_time < run_time_s()) {
    end_time = 0;
    soMsg_cuas_timeout(*my_td);
    cuas_state = CUAS_TIME_OUT;
    cuas_active = false;
    cuas_DISABLE_cb();
  }
}
#define CI(cb) static_cast<configItem>(cb)

bool cu_auto_set_check(const fer_sbT *fsb) {
  if (end_time == 0)
    return false;

  if (FER_SB_ADDR_IS_CENTRAL(fsb)) {
    u32 cu = FER_SB_GET_DEVID(fsb);
    config_save_item_n_u32(CI(CB_CUID), cu);
    config_item_modified(CI(CB_CUID));
    end_time = 0;
    soMsg_cuas_done(*my_td);
    cuas_state = CUAS_SUCCESS;
    config_save_item_n_u32(CI(CB_CUID), cu);
    config_item_modified(CI(CB_CUID));
    cuas_active = false;
    cuas_DISABLE_cb();
    return true;
  }

  return false;
}
