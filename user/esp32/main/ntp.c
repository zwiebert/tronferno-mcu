/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "string.h"

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "time.h"
#include "main/rtc.h"

#include "main/inout.h"
#include "main/fer.h"
#include "main/config.h"
#include "main/rtc.h"
#include "main/config.h"


//#include <sntp.h>

#define printf ets_printf

static time_t last_ntp_time;

void setup_ntp(void) {
  static int once;
  if (once == 0) {
    once = 1;
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

  }
}

#define NO_ADJUST_UNTIL (SECS_PER_HOUR) // if ntp is updated too often, don't adjust to avoid wrong values and burning out persistent storage
#define ADJUST_TOLERANCE_MS 100    // allow tolerance to avoid burning out persistent storage


bool ntp_set_system_time(void) {

  return false;
}

bool ntp_update_system_time(unsigned interval_seconds) {
  if (last_ntp_time == 0 || time(NULL) >= last_ntp_time + interval_seconds) {
    ntp_set_system_time();
    return true;
  }
  return false;

}
