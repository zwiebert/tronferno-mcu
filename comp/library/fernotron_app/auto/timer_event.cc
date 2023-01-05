#include "app_config/proj_app_cfg.h"

#include <fernotron/auto/fau_tevent.hh>
#include "fernotron/auto/fau_tdata_store.h"
#include "fernotron_trx/astro.h"
#include "fernotron/fer_main.h"
#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include "time.h"
#include "fernotron/pos/shutter_pct.h"
#include <fernotron/fer_main.h>

#include <stdlib.h>
#include <string.h>

#define MAX_GRP 7
#define MAX_MBR 7

void Fer_TimerEvent::fer_am_updateTimerEventTime(const time_t &now_time) {
  const fer_au_minutesT next_min = next_event;
  const fer_au_minutesT now_min = fer_au_mintsFromTime(&now_time);
  const time_t midnight = ((60 * 24) - now_min) * 60 + now_time;

  if (next_min == MINUTES_DISABLED) {
    next_event_time = midnight;
  } else {
    next_event_time = fer_au_timeFromMints(&now_time, next_min);
  }
}


void Fer_TimerEvent::fer_am_updateTimerEvent(const time_t &now_time) {
  const time_t now_time_next = now_time;
  fer_am_get_next_timer_event(&now_time_next);
  fer_am_updateTimerEventTime(now_time);
}

bool Fer_TimerEvent::te_isEventNow(const time_t &now_time) {

  if (te_isDisabled()) {
    return false; // there is no more event today
  }

  if (now_time < te_getEventTime()) {
    return false; // it's too early for next event to occur
  }

  if (now_time > (te_getEventTime() + 30)) {
    fer_am_updateTimerEvent(now_time);
    return false; // the latest event was missed somehow
  }

  return te_matchMinute(now_time);
}

static int set_earliest(uint8_t g, uint8_t m, fer_au_minutesT *earliest, const struct tm *tm_now, fer_au_minutesT minutes_now, Fer_GmSet *gm) {
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

static bool fer_am_get_next_timer_event_earliest(Fer_GmSet *mask_result, fer_au_minutesT *earliest_result, const struct tm *tm_now,
    fer_au_minutesT minutes_now) {
  uint8_t g;
  fer_au_minutesT earliest = MINUTES_DISABLED;
  bool result = false;

  if (set_earliest(0, 0, &earliest, tm_now, minutes_now, mask_result)) {
    for (g = 1; g <= MAX_GRP; ++g) {
      (*mask_result)[g] = fer_usedMemberMask[g];
    }
  }

  for (g = 1; g <= MAX_GRP; ++g) {
    if (set_earliest(g, 0, &earliest, tm_now, minutes_now, mask_result)) {
      (*mask_result)[g] = fer_usedMemberMask[g];
    }
  }

  for (auto it = fer_usedMemberMask.begin(1); it; ++it) {
    uint8_t g = it.getG(), m = it.getM();
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

bool Fer_TimerEvent::te_matchMinute(const time_t &now_time) {
    struct tm *tm = localtime(&now_time);
    return next_event == fer_au_mintsFromTm(tm);
}

bool Fer_TimerEvent::fer_am_get_next_timer_event(const time_t *now_time) {
  fer_au_minutesT earliest = MINUTES_DISABLED;
  Fer_GmSet existing_members;

  *this = Fer_TimerEvent();


  struct tm tm_now;
  localtime_r(now_time, &tm_now);
  fer_au_minutesT minutes_now = tm_now.tm_hour * 60 + tm_now.tm_min;

  if (!fer_am_get_next_timer_event_earliest(&existing_members, &earliest, &tm_now, minutes_now))
    return false;

  next_event = earliest;

  for (auto it = fer_usedMemberMask.begin(); it; ++it) {
    uint8_t g = it.getG(), m = it.getM();
    if (manual_bits.getMember(g, m))
      continue;

    Fer_TimerMinutes timi;
    uint8_t g2 = g, m2 = m;
    Fer_TimerData td;
    if (!(fer_stor_timerData_load(&td, &g2, &m2, true) && fer_au_get_timer_minutes_from_timer_data_tm(&timi, &td, &tm_now))) {
      continue; // should not happen
    }

    fer_au_minutesT temp = fer_au_get_earliest_from_timer_minutes(&timi, minutes_now);
    if (temp <= earliest) {

      if (timi.minutes[FER_MINTS_DAILY_UP] == earliest || timi.minutes[FER_MINTS_WEEKLY_UP] == earliest) {
        te_getMaskUp()->setMember(g, m);

      }
      if (timi.minutes[FER_MINTS_ASTRO] == earliest || timi.minutes[FER_MINTS_DAILY_DOWN] == earliest || timi.minutes[FER_MINTS_WEEKLY_DOWN] == earliest) {
        te_getMaskDown()->setMember(g, m);
      }
    }
  }

  return true;
}
