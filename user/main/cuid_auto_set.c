/*
 * cuid_auto_scan.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */

#include "cuid_auto_set.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cli/cli.h"
#include "config/config.h"
#include "fernotron/fer_code.h"
#include "main/common.h"
#include "userio/status_output.h"
#include "main/rtc.h"



static bool cuas_active;
static time_t end_time;

bool ICACHE_FLASH_ATTR cu_auto_set(uint16_t id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;

  if (timeout_secs > 0) {
    end_time = run_time(NULL) + timeout_secs;
    last_received_sender.data[0] = 0;
    cuas_active = true;
    so_output_message(SO_CUAS_START, &id);
  }
  return false;
}

bool ICACHE_FLASH_ATTR cu_auto_set_check_timeout() {
  if (end_time == 0)
    return false;

  if (end_time < run_time(NULL)) {
      end_time = 0;
      so_output_message(SO_CUAS_TIMEOUT, NULL);
      cuas_active = false;
      return true;
    }
  return false;
}

bool ICACHE_FLASH_ATTR cu_auto_set_check(fer_sender_basic *fsb) {
  if (end_time == 0)
    return false;

 if (FSB_ADDR_IS_CENTRAL(fsb)) {
    uint32_t cu = FSB_GET_DEVID(fsb);
    FSB_PUT_DEVID(&default_sender, cu);
    C.fer_centralUnitID = cu;
    end_time = 0;
    so_output_message(SO_CUAS_DONE, NULL);
    save_config(CONFIG_CUID);
    cuas_active = false;
    return true;
  }

  return false;
}
