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

static timer_event_t teud[2];

time_t next_event;

minutes_t get_now() {
  time_t timer = time(NULL);
  struct tm *tm = localtime(&timer);
  minutes_t minutes_now = tm->tm_hour * 60 + tm->tm_min;
  return minutes_now;
}

static void set_next_event() {
  minutes_t next_min = MIN(teud[0].next_event, teud[1].next_event);
  minutes_t now_min = get_now();
  time_t now = time(NULL);
  time_t midnight = ((60 * 24) - now_min) * 60 + now;

  if (next_min == MINUTES_DISABLED) {
    next_event = midnight;
  } else {
    next_event = (next_min - now_min) * 60 + now;
  }
}

void fau_getnextTimerEvent() {
  get_next_timer_event(&teud[0], &teud[1]);
  set_next_event();
}


void timer_state_loop_evt(void) {
  timer_event_t *te;
  bool get_new_event = false;
  int k;
  if (time(NULL) < next_event)
    return;

  minutes_t now_min = get_now();

  for (k = 0; k < 2; ++k) {
    te = &teud[k];
    if (te->next_event != MINUTES_DISABLED) {
      if (te->next_event != now_min)
        continue;

      ferPos_registerMovingShutters(&te->matching_members, te_is_up(te) ? fer_cmd_UP : fer_cmd_DOWN);
      get_new_event = true;
    }
  }
  if (get_new_event)
    fau_getnextTimerEvent();
}

void timer_state_loop(void) {
  static bool initialized;
  timer_event_t *teu = &teud[0], *ted = &teud[1], *te;
  int k;

  i16 new_minute = rtc_get_next_minute();

  // FIXME: should also check for changed rtc and longitude/latitude/tz settings
  if (new_minute == 0 || !initialized || timer_data_changed) {
    get_next_timer_event(teu, ted);
    initialized = true;
    timer_data_changed = false;
  }


  if (new_minute < 0)
    return;

  for (k = 0; k < 2; ++k) {
    te = &teud[k];
    if (te->next_event != MINUTES_DISABLED) {
      if (te->next_event != new_minute) {
        initialized = false;
        continue;
      }
      ferPos_registerMovingShutters(&te->matching_members, te_is_up(te) ? fer_cmd_UP : fer_cmd_DOWN);
    }
  }
}

