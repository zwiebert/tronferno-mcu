#include "app_config/proj_app_cfg.h"

#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/auto/fau_tdata_store.h"
#include "fernotron/pos/shutter_pct.h"
#include "fernotron_trx/astro.h"
#include "fernotron/fer_main.h"

#include "debug/dbg.h"
#include "utils_misc/int_macros.h"
#include <utils_time/ut_constants.hh>

#include "time.h"
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

  if (!tm) {
    const time_t now = time(NULL);
    tm = localtime(&now);
  }

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

