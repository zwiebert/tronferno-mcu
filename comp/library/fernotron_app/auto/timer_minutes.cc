#include "app/config/proj_app_cfg.h"

#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "app/rtc.h"
#include "fernotron/astro.h"
#include "app/settings/config.h"
#include "debug/dbg.h"
#include "misc/int_macros.h"
#include "time.h"
#include "fernotron/pos/shutter_pct.h"

#include <stdlib.h>
#include <string.h>


#ifndef DISTRIBUTION
#define DB_INFO 0
#define DT(x)
#define D(x)
#else
#define DB_INFO 0
#define DT(x)
#define D(x)
#endif

static bool 
timer_to_minutes(minutes_t *result, const char *ts) {
  char buf[3];
  DT(ets_printf("%s: ts=%s\n", __FUNCTION__, ts));

  if (*ts == '-') {
    *result = MINUTES_DISABLED;
    return false;
  }

  buf[2] = '\0';
  buf[0] = ts[0];
  buf[1] = ts[1];
  *result = atoi(buf) * 60;
  buf[0] = ts[2];
  buf[1] = ts[3];
  *result += atoi(buf);
  return true;
}

bool fau_get_timer_minutes_from_timer_data_tm(timer_minutes_t *timi, const timer_data_t *tdp, const struct tm *tm) {

  precond(timi && tdp);

  const char *p = 0;
  int i;

  // weekly strings starts with monday, not sunday
  u8 weekday = (tm->tm_wday == 0) ? 6 : tm->tm_wday - 1;

  for (i = 0; i < SIZE_MINTS; ++i)
    timi->minutes[i] = MINUTES_DISABLED;

  if (td_is_daily(tdp)) {
    p = tdp->daily;

    if (timer_to_minutes(&timi->minutes[DAILY_UP_MINTS], p))
      p += 4;
    else
      ++p;

    timer_to_minutes(&timi->minutes[DAILY_DOWN_MINTS], p);

  }

  if (td_is_weekly(tdp)) {
    int i;
    minutes_t up_mints = MINUTES_DISABLED, down_mints = MINUTES_DISABLED;
    p = tdp->weekly;
    D(ets_printf("weekly:<%s>\n", p));

    for (i = 0; i < 7 /* week */; ++i) {
      if (i == 0 || *p != '+') {
        if (timer_to_minutes(&up_mints, p))
          p += 4;
        else
          ++p;

        if (timer_to_minutes(&down_mints, p))
          p += 4;
        else
          ++p;
      } else if (*p == '+') {
        ++p;
      }

      if (i == weekday) {
        timi->minutes[WEEKLY_UP_MINTS] = up_mints;
        timi->minutes[WEEKLY_DOWN_MINTS] = down_mints;
        break;
      }
    }
  }

  if (td_is_astro(tdp)) {
    timi->minutes[ASTRO_MINTS] = astro_calc_minutes(tm) + tdp->astro;
  }

  postcond(timi->minutes[ASTRO_MINTS] == MINUTES_DISABLED || timi->minutes[ASTRO_MINTS] < MINT_PER_DAY);

  return true;
}

bool fau_get_timer_minutes_tm(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard, const struct tm *tm) {

  precond(timi && group && member);

  timer_data_t td;
  if (read_timer_data(&td, group, member, wildcard)) {
    return fau_get_timer_minutes_from_timer_data_tm(timi, &td, tm);
  }

  return false;
}

bool 
fau_get_timer_minutes_tim(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard, const time_t *now_time) {
  struct tm *tm = localtime(now_time);
  return fau_get_timer_minutes_tm(timi, group, member, wildcard, tm);
}

bool
fau_get_timer_minutes_now(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard) {
  time_t now_time = time(NULL);
  return fau_get_timer_minutes_tim(timi, group, member, wildcard, &now_time);
}

minutes_t
fau_get_earliest_from_timer_minutes(timer_minutes_t *timi, minutes_t now) {
  u8 i;
  minutes_t last = MINUTES_DISABLED;
  for (i=0; i < SIZE_MINTS; ++i) {
    if (timi->minutes[i] <= now)
      continue; // timer not in future

    last = MIN(last, timi->minutes[i]);
  }
  return last;
}

