/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */

#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/uart.h"
#include <mem.h>

#include "config/config.h"
#include "app/rtc.h"
#include "config/config.h"
#include <sntp.h>
#include "txtio/inout.h"
#include "misc/int_types.h"

extern int ets_uart_printf(const char *fmt, ...);
#define printf ets_uart_printf

static time_t last_ntp_time;

void  ntp_setup(void) {
  static int once;
  if (once == 0) {
    once = 1;
    ip_addr_t *addr = (ip_addr_t *) os_zalloc(sizeof(ip_addr_t));
    sntp_setservername(0, "us.pool.ntp.org"); // set server 0 by domain name
    sntp_setservername(1, "ntp.sjtu.edu.cn"); // set server 1 by domain name
    ipaddr_aton("210.72.145.44", addr);
    sntp_setserver(2, addr); // set server 2 by IP address
    sntp_set_timezone((int) 0);
    sntp_init();
    os_free(addr);
  }
}

#define NO_ADJUST_UNTIL (SECS_PER_HOUR) // if ntp is updated too often, don't adjust to avoid wrong values and burning out persistent storage
#define ADJUST_TOLERANCE_MS 100    // allow tolerance to avoid burning out persistent storage

void  auto_adjust_time(time_t rtc_time, time_t ntp_time) {

  if (last_ntp_time != 0 && last_ntp_time + NO_ADJUST_UNTIL < ntp_time) {
    i32 diff_time, interval_time;
    double interval_days;
    i32 adj_ms_per_day;
    i32 diff_ms;

    diff_time = difftime(rtc_time, ntp_time);
    diff_ms = difftime(rtc_time, ntp_time) * 1000L;
    interval_time = difftime(ntp_time, last_ntp_time);
    interval_days = (ntp_time - last_ntp_time) / (double) ONE_DAY;
    if (abs(adj_ms_per_day = diff_ms / interval_days) > ADJUST_TOLERANCE_MS) {
      C.app_rtcAdjust += adj_ms_per_day;
      save_config(~0);
    }
  }
}

bool  ntp_set_system_time(void) {
  u32 time_stamp = sntp_get_current_timestamp();
  if (time_stamp != 0) {
    time_t rtc_time, ntp_time;
    bool autoAdjust = (rtc_last_time_source == RTC_SRC_NTP);

    rtc_time = time(NULL);
    rtc_set_system_time(sntp_get_current_timestamp() - 946684800, RTC_SRC_NTP);
    ntp_time = time(NULL);
    if (autoAdjust) {
#ifdef RTC_AUTO_ADJUST
      auto_adjust_time(rtc_time, ntp_time);
#endif
    }
    if (C.app_verboseOutput >= vrb1) {
      printf("ntp stamp: %lu, %s (adjust=%ld, interval=%d, diff=%d)\n", sntp_get_current_timestamp(), sntp_get_real_time(sntp_get_current_timestamp()),
          (long) C.app_rtcAdjust, (int) difftime(ntp_time, last_ntp_time), (int) (rtc_time - ntp_time));
    }
    last_ntp_time = ntp_time;
    return true;
  }
  return false;
}

bool  ntp_update_system_time(unsigned interval_seconds) {
  if (last_ntp_time == 0 || time(NULL) >= last_ntp_time + interval_seconds) {
    ntp_set_system_time();
    return true;
  }
  return false;

}
