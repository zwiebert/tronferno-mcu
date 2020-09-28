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
timer_to_minutes(fer_au_minutesT *result, const char *ts) {
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

bool fer_au_get_timer_minutes_from_timer_data_tm(Fer_TimerMinutes *timi, const Fer_TimerData *tdp, const struct tm *tm) {

  precond(timi && tdp);

  const char *p = 0;
  int i;

  // weekly strings starts with monday, not sunday
  u8 weekday = (tm->tm_wday == 0) ? 6 : tm->tm_wday - 1;

  for (i = 0; i < FER_MINTS_size; ++i)
    timi->minutes[i] = MINUTES_DISABLED;

  if (tdp->hasDaily()) {
    p = tdp->getDaily();

    if (timer_to_minutes(&timi->minutes[FER_MINTS_DAILY_UP], p))
      p += 4;
    else
      ++p;

    timer_to_minutes(&timi->minutes[FER_MINTS_DAILY_DOWN], p);

  }

  if (tdp->hasWeekly()) {
    int i;
    fer_au_minutesT up_mints = MINUTES_DISABLED, down_mints = MINUTES_DISABLED;
    p = tdp->getWeekly();
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
        timi->minutes[FER_MINTS_WEEKLY_UP] = up_mints;
        timi->minutes[FER_MINTS_WEEKLY_DOWN] = down_mints;
        break;
      }
    }
  }

  if (tdp->hasAstro()) {
    timi->minutes[FER_MINTS_ASTRO] = fer_astro_calc_minutes(tm) + tdp->getAstro();
  }

  postcond(timi->minutes[FER_MINTS_ASTRO] == MINUTES_DISABLED || timi->minutes[FER_MINTS_ASTRO] < MINT_PER_DAY);

  return true;
}

bool fer_au_get_timer_minutes_tm(Fer_TimerMinutes *timi, u8 *group, u8 *member, bool wildcard, const struct tm *tm) {

  precond(timi && group && member);

  Fer_TimerData td;
  if (fer_stor_timerData_load(&td, group, member, wildcard)) {
    return fer_au_get_timer_minutes_from_timer_data_tm(timi, &td, tm);
  }

  return false;
}

bool 
fer_au_get_timer_minutes_tim(Fer_TimerMinutes *timi, u8 *group, u8 *member, bool wildcard, const time_t *now_time) {
  struct tm *tm = localtime(now_time);
  return fer_au_get_timer_minutes_tm(timi, group, member, wildcard, tm);
}

bool
fer_au_get_timer_minutes_now(Fer_TimerMinutes *timi, u8 *group, u8 *member, bool wildcard) {
  time_t now_time = time(NULL);
  return fer_au_get_timer_minutes_tim(timi, group, member, wildcard, &now_time);
}

fer_au_minutesT
fer_au_get_earliest_from_timer_minutes(Fer_TimerMinutes *timi, fer_au_minutesT now) {
  u8 i;
  fer_au_minutesT last = MINUTES_DISABLED;
  for (i=0; i < FER_MINTS_size; ++i) {
    if (timi->minutes[i] <= now)
      continue; // timer not in future

    last = MIN(last, timi->minutes[i]);
  }
  return last;
}

