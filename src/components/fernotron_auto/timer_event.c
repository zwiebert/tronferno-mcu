#include "app_config/proj_app_cfg.h"

#include "fau_tevent.h"
#include "fernotron_auto/fau_tdata_store.h"
#include "app/rtc.h"
#include "fernotron/astro.h"
#include "config/config.h"
#include "debug/debug.h"
#include "misc/int_macros.h"
#include "time.h"
#include "fernotron_pos/shutter_pct.h"
#include "cli_app/cli_fer.h"

#include <stdlib.h>
#include <string.h>

#define MAX_GRP 7
#define MAX_MBR 7

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
  fam_get_next_timer_event(&next_event_te, &now_time_next);
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
    fam_get_next_timer_event(te, &now_time);
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


static int set_earliest(u8 g, u8 m, minutes_t *earliest, const struct tm *tm_now, minutes_t minutes_now, gm_bitmask_t *gm) {
  int result = 0;

  timer_minutes_t timi;
  if (fau_get_timer_minutes_tm(&timi, &g, &m, false, tm_now)) {
    minutes_t temp = fau_get_earliest_from_timer_minutes(&timi, minutes_now);
    if (temp == *earliest)
      result = 1;
    else if (temp < *earliest) {
      *earliest = temp;
      gm_Clear(gm);
      result = 2;
    }
  }

  return result;
}


static bool fam_get_next_timer_event_earliest(gm_bitmask_t *mask_result, minutes_t *earliest_result, const struct tm *tm_now, minutes_t minutes_now) {
  u8 g, m;
  minutes_t earliest = MINUTES_DISABLED;

  if (set_earliest(0, 0, &earliest, tm_now, minutes_now, mask_result)) {
    for (g = 1; g <= MAX_GRP; ++g) {
      gm_SetByte(mask_result, g, gm_GetByte(&C.fer_usedMemberMask, g));
    }
  }

  for (g = 1; g <= MAX_GRP; ++g) {
    if (set_earliest(g, 0, &earliest, tm_now, minutes_now, mask_result)) {
      gm_SetByte(mask_result, g, gm_GetByte(&C.fer_usedMemberMask, g));
    }
  }

  for ((g = 1), (m = ~0); gm_getNext(&C.fer_usedMemberMask, &g, &m);) {
    if (gm_GetBit(&manual_bits, g, m))
      continue;

    if (set_earliest(g, m, &earliest, tm_now, minutes_now, mask_result)) {
      gm_SetBit(mask_result, g, m);
    }
  }
  if (earliest_result)
    *earliest_result = earliest;
  return (earliest != MINUTES_DISABLED);
}

bool fam_get_next_timer_event(timer_event_t *evt, const time_t *now_time) {
  precond(evt);
  u8 g, m;
  minutes_t earliest = MINUTES_DISABLED;
  gm_bitmask_t existing_members = { 0, };

  *evt = (timer_event_t ) { .next_event = MINUTES_DISABLED, };

  const struct tm *tm_now = localtime(now_time);
  minutes_t minutes_now = tm_now->tm_hour * 60 + tm_now->tm_min;

  if (!fam_get_next_timer_event_earliest(&existing_members, &earliest, tm_now, minutes_now))
    return false;

  evt->next_event = earliest;

  for ((g = 0), (m = ~0); gm_getNext(&existing_members, &g, &m);) {
    if (gm_GetBit(&manual_bits, g, m))
      continue;

    timer_minutes_t timi;
    u8 g2 = g, m2 = m;
    if (!fau_get_timer_minutes_tm(&timi, &g2, &m2, true, tm_now))
      continue; // should not happen

    minutes_t temp = fau_get_earliest_from_timer_minutes(&timi, minutes_now);
    if (temp <= earliest) {

      if (timi.minutes[DAILY_UP_MINTS] == earliest || timi.minutes[WEEKLY_UP_MINTS] == earliest) {
        gm_SetBit(te_getMaskUp(evt), g, m);

      }
      if (timi.minutes[ASTRO_MINTS] == earliest || timi.minutes[DAILY_DOWN_MINTS] == earliest || timi.minutes[WEEKLY_DOWN_MINTS] == earliest) {
        gm_SetBit(te_getMaskDown(evt), g, m);
      }
    }
  }

  return true;
}

