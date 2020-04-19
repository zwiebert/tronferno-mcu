#include "app_config/proj_app_cfg.h"

#include "timer_state.h"
#include "fernotron_auto/timer_data.h"
#include "app/rtc.h"
#include "fernotron/astro.h"
#include "config/config.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include "time.h"
#include "fernotron_pos/shutter_pct.h"

#include <stdlib.h>
#include <string.h>

static timer_event_t next_event_te;

time_t next_event_time;

minutes_t get_now_min() {
  time_t timer = time(NULL);
  struct tm *tm = localtime(&timer);
  return tm_DMin(tm);
}

minutes_t get_min(time_t *timer) {
  struct tm *tm = localtime(timer);
  return tm_DMin(tm);
}

static void fam_updateTimerEventTime(time_t *now_time) {
  minutes_t next_min = next_event_te.next_event;
  minutes_t now_min = get_min(now_time);
  time_t midnight = ((60 * 24) - now_min) * 60 + *now_time;

  if (next_min == MINUTES_DISABLED) {
    next_event_time = midnight;
  } else {
    next_event_time = (next_min - now_min) * 60 + *now_time;
  }
}

void fam_updateTimerEvent() {
  time_t now_time = time(NULL);
  time_t now_time_next = now_time + 60; // make sure we don't get the current event again
  fam_get_next_timer_event_te(&next_event_te, &now_time_next);
  fam_updateTimerEventTime(&now_time);
}

// new loop (ESP32)
void fam_loop(void) {
  timer_event_t *te = &next_event_te;

  if (te_isDisabled(te))
    return;
  if (time(NULL) < next_event_time)
    return;
  if (te->next_event != get_now_min())
    return;

  if (simPos_registerMovingShutters(te_getMaskUp(te), fer_cmd_UP) >= 0 || simPos_registerMovingShutters(te_getMaskUp(te), fer_cmd_DOWN) >= 0)
    fam_updateTimerEvent();
}

// old loop (ESP8266)
void fam_loop_old(void) {
  static bool initialized;
  timer_event_t *te = &next_event_te;

  i16 new_minute = rtc_get_next_minute();

  // FIXME: should also check for changed rtc and longitude/latitude/tz settings
  if (new_minute == 0 || !initialized || timer_data_changed) {
    time_t now_time = time(NULL);
    now_time += 60; // make sure we don't get the current event again
    fam_get_next_timer_event_te(te, &now_time);
    initialized = true;
    timer_data_changed = false;
  }

  if (te_isDisabled(te))
    return;
  if (te->next_event != get_now_min())
    return;

  simPos_registerMovingShutters(te_getMaskUp(te), fer_cmd_UP);
  simPos_registerMovingShutters(te_getMaskUp(te), fer_cmd_DOWN);

}

