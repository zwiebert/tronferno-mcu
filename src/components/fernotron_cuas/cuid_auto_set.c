/*
 * cuid_auto_scan.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include <string.h>
#include "app/proj_app_cfg.h"

#include "cuid_auto_set.h"

#include "cli/cli.h"
#include "config/config.h"
#include "main/common.h"
#include "userio_app/status_output.h"
#include "main/rtc.h"
#include "app/fernotron.h"

#include "fernotron/fer_msg_rx.h"
#include "fernotron/fer_msg_attachment.h"


static cuas_state_T cuas_state;

cuas_state_T cuas_getState() {
  return cuas_state;
}

static bool cuas_active;
static time_t end_time;

bool  cu_auto_set(u16 id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;

  if (timeout_secs > 0) {
    end_time = run_time(NULL) + timeout_secs;
    last_received_sender.data[0] = 0;
    cuas_active = true;
    so_output_message(SO_CUAS_START, &id);
    cuas_state = CUAS_SCANNING;
  }
  return false;
}

bool  cu_auto_set_check_timeout() {
  if (end_time == 0)
    return false;

  if (end_time < run_time(NULL)) {
      end_time = 0;
      so_output_message(SO_CUAS_TIMEOUT, NULL);
      cuas_state = CUAS_TIME_OUT;
      cuas_active = false;
      return true;
    }
  return false;
}

bool  cu_auto_set_check(const fsbT *fsb) {
  if (end_time == 0)
    return false;

 if (FSB_ADDR_IS_CENTRAL(fsb)) {
    u32 cu = FSB_GET_DEVID(fsb);
    FSB_PUT_DEVID(&default_sender, cu);
    C.fer_centralUnitID = cu;
    end_time = 0;
    so_output_message(SO_CUAS_DONE, NULL);
    cuas_state = CUAS_SUCCESS;
    save_config_item(CB_CUID);
    cuas_active = false;
    return true;
  }

  return false;
}
