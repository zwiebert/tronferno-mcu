#include "app/config/proj_app_cfg.h"

#include "fernotron/auto/fau_tevent.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "app/rtc.h"
#include "fernotron/astro.h"
#include "app/settings/config.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "time.h"
#include "fernotron/pos/shutter_pct.h"
#include <fernotron/fer_main.h>

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

minutes_t get_min(const time_t *timer) {
  struct tm *tm = localtime(timer);
  return tm_DMin(tm);
}

static void fam_updateTimerEventTime(const time_t *now_time) {
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
  time_t now_time_next = now_time + 60; // XXX: make sure we don't get the current event again
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

  simPos_registerMovingShutters(te_getMaskUp(te), fer_cmd_UP);
  simPos_registerMovingShutters(te_getMaskDown(te), fer_cmd_DOWN);
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
  simPos_registerMovingShutters(te_getMaskDown(te), fer_cmd_DOWN);

}


static int set_earliest(u8 g, u8 m, minutes_t *earliest, const struct tm *tm_now, minutes_t minutes_now, GmBitMask *gm) {
  int result = 0;

  timer_minutes_t timi;
  if (fau_get_timer_minutes_tm(&timi, &g, &m, false, tm_now)) {
    minutes_t temp = fau_get_earliest_from_timer_minutes(&timi, minutes_now);
    if (temp == MINUTES_DISABLED)
      result = 0;
    else if (temp == *earliest)
      result = 1;
    else if (temp < *earliest) {
      *earliest = temp;
      gm->clear();
      result = 2;
    }
  }

  return result;
}


static bool fam_get_next_timer_event_earliest(GmBitMask *mask_result, minutes_t *earliest_result, const struct tm *tm_now, minutes_t minutes_now) {
  u8 g;
  minutes_t earliest = MINUTES_DISABLED;
  bool result = false;

  if (set_earliest(0, 0, &earliest, tm_now, minutes_now, mask_result)) {
    for (g = 1; g <= MAX_GRP; ++g) {
      (*mask_result)[g] = C.fer_usedMemberMask[g];
    }
  }

  for (g = 1; g <= MAX_GRP; ++g) {
    if (set_earliest(g, 0, &earliest, tm_now, minutes_now, mask_result)) {
      (*mask_result)[g] = C.fer_usedMemberMask[g];
    }
  }

  for (auto it = C.fer_usedMemberMask.begin(1); it; ++it) {
    u8 g = it.getG(), m = it.getM();
    if (manual_bits.getBit(g, m))
      continue;

    if (set_earliest(g, m, &earliest, tm_now, minutes_now, mask_result)) {
      mask_result->setBit(g, m);
    }
  }

  if (mask_result->isAllClear())
    earliest = MINUTES_DISABLED;

  result = (earliest != MINUTES_DISABLED);

  if (result && earliest_result)
    *earliest_result = earliest;

  postcond(result == !mask_result->isAllClear());
  return result;
}


bool fam_get_next_timer_event(timer_event_t *evt, const time_t *now_time) {
  precond(evt);

  minutes_t earliest = MINUTES_DISABLED;
  GmBitMask existing_members ;

  *evt = (timer_event_t ) { .next_event = MINUTES_DISABLED, };
  struct tm tm_now;
  localtime_r(now_time, &tm_now);
  minutes_t minutes_now = tm_now.tm_hour * 60 + tm_now.tm_min;

  if (!fam_get_next_timer_event_earliest(&existing_members, &earliest, &tm_now, minutes_now))
    return false;

  evt->next_event = earliest;

  for (auto it = C.fer_usedMemberMask.begin(); it; ++it) {
    u8 g = it.getG(), m = it.getM();
    if (manual_bits.getBit(g, m))
      continue;

    timer_minutes_t timi;
    u8 g2 = g, m2 = m;
    if (!fau_get_timer_minutes_tm(&timi, &g2, &m2, true, &tm_now))
      continue; // should not happen

    minutes_t temp = fau_get_earliest_from_timer_minutes(&timi, minutes_now);
    if (temp <= earliest) {

      if (timi.minutes[DAILY_UP_MINTS] == earliest || timi.minutes[WEEKLY_UP_MINTS] == earliest) {
        te_getMaskUp(evt)->setBit(g, m);

      }
      if (timi.minutes[ASTRO_MINTS] == earliest || timi.minutes[DAILY_DOWN_MINTS] == earliest || timi.minutes[WEEKLY_DOWN_MINTS] == earliest) {
        te_getMaskDown(evt)->setBit(g, m);
      }
    }
  }

  return true;
}

