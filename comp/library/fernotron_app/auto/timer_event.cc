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

static Fer_TimerEvent next_event_te;

time_t next_event_time;

fer_au_minutesT fer_au_mintsNow() {
  time_t timer = time(NULL);
  struct tm *tm = localtime(&timer);
  return fer_au_mintsFromTm(tm);
}

fer_au_minutesT fer_au_mintsFromTime(const time_t *timer) {
  struct tm *tm = localtime(timer);
  return fer_au_mintsFromTm(tm);
}

static void fer_am_updateTimerEventTime(const time_t *now_time) {
  fer_au_minutesT next_min = next_event_te.next_event;
  fer_au_minutesT now_min = fer_au_mintsFromTime(now_time);
  time_t midnight = ((60 * 24) - now_min) * 60 + *now_time;

  if (next_min == MINUTES_DISABLED) {
    next_event_time = midnight;
  } else {
    next_event_time = (next_min - now_min) * 60 + *now_time;
  }
}

void fer_am_updateTimerEvent() {
  time_t now_time = time(NULL);
  time_t now_time_next = now_time + 60; // XXX: make sure we don't get the current event again
  fer_am_get_next_timer_event(&next_event_te, &now_time_next);
  fer_am_updateTimerEventTime(&now_time);
}

// new loop (ESP32)
void fer_am_loop(void) {
  Fer_TimerEvent *te = &next_event_te;

  if (te_isDisabled(te))
    return;
  if (time(NULL) < next_event_time)
    return;
  if (te->next_event != fer_au_mintsNow())
    return;

  fer_simPos_registerMovingShutters(te_getMaskUp(te), fer_if_cmd_UP);
  fer_simPos_registerMovingShutters(te_getMaskDown(te), fer_if_cmd_DOWN);
  fer_am_updateTimerEvent();
}

static int set_earliest(u8 g, u8 m, fer_au_minutesT *earliest, const struct tm *tm_now, fer_au_minutesT minutes_now, Fer_GmSet *gm) {
  Fer_TimerData td;
  if (!fer_stor_timerData_load(&td, &g, &m, false))
    return 0;

  Fer_TimerMinutes timi;
  if (!fer_au_get_timer_minutes_from_timer_data_tm(&timi, &td, tm_now))
    return 0;

    fer_au_minutesT temp = fer_au_get_earliest_from_timer_minutes(&timi, minutes_now);
    if (temp == MINUTES_DISABLED)
      return 0;

    if (temp == *earliest)
      return 1;

    if (temp < *earliest) {
      *earliest = temp;
      gm->clear();
      return 2;
    }

  return 0;
}


static bool fer_am_get_next_timer_event_earliest(Fer_GmSet *mask_result, fer_au_minutesT *earliest_result, const struct tm *tm_now, fer_au_minutesT minutes_now) {
  u8 g;
  fer_au_minutesT earliest = MINUTES_DISABLED;
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
    if (manual_bits.getMember(g, m))
      continue;

    if (set_earliest(g, m, &earliest, tm_now, minutes_now, mask_result)) {
      mask_result->setMember(g, m);
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


bool fer_am_get_next_timer_event(Fer_TimerEvent *evt, const time_t *now_time) {
  precond(evt);

  fer_au_minutesT earliest = MINUTES_DISABLED;
  Fer_GmSet existing_members ;

  *evt = (Fer_TimerEvent ) { .next_event = MINUTES_DISABLED, };
  struct tm tm_now;
  localtime_r(now_time, &tm_now);
  fer_au_minutesT minutes_now = tm_now.tm_hour * 60 + tm_now.tm_min;

  if (!fer_am_get_next_timer_event_earliest(&existing_members, &earliest, &tm_now, minutes_now))
    return false;

  evt->next_event = earliest;

  for (auto it = C.fer_usedMemberMask.begin(); it; ++it) {
    u8 g = it.getG(), m = it.getM();
    if (manual_bits.getMember(g, m))
      continue;


    Fer_TimerMinutes timi;
    u8 g2 = g, m2 = m;
    Fer_TimerData td;
    if (!(fer_stor_timerData_load(&td, &g2, &m2, true) && fer_au_get_timer_minutes_from_timer_data_tm(&timi, &td, &tm_now))) {
      continue; // should not happen
    }

    fer_au_minutesT temp = fer_au_get_earliest_from_timer_minutes(&timi, minutes_now);
    if (temp <= earliest) {

      if (timi.minutes[FER_MINTS_DAILY_UP] == earliest || timi.minutes[FER_MINTS_WEEKLY_UP] == earliest) {
        te_getMaskUp(evt)->setMember(g, m);

      }
      if (timi.minutes[FER_MINTS_ASTRO] == earliest || timi.minutes[FER_MINTS_DAILY_DOWN] == earliest || timi.minutes[FER_MINTS_WEEKLY_DOWN] == earliest) {
        te_getMaskDown(evt)->setMember(g, m);
      }
    }
  }

  return true;
}

