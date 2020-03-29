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

extern gm_bitmask_t manual_bits;

bool 
get_timer_minutes_tm(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard, struct tm *tm) {

  precond(timi && group && member);

  timer_data_t td = td_initializer; //?
  const char *p = 0;
  int i;

  // weekly strings starts with monday, not sunday
  u8 weekday = (tm->tm_wday == 0) ? 6 : tm->tm_wday - 1;

  if (!read_timer_data(&td, group, member, wildcard))
    return false;

  for (i=0; i < SIZE_MINTS; ++i)
    timi->minutes[i] = MINUTES_DISABLED;


  if (td_is_daily(&td)) {
    p = td.daily;

    if (timer_to_minutes(&timi->minutes[DAILY_UP_MINTS], p))
      p += 4;
    else
      ++p;

    timer_to_minutes(&timi->minutes[DAILY_DOWN_MINTS], p);

  }

  if (td_is_weekly(&td)) {
    int i;
    minutes_t up_mints = MINUTES_DISABLED, down_mints = MINUTES_DISABLED;
    p = td.weekly;
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

  if (td_is_astro(&td)) {
    timi->minutes[ASTRO_MINTS] = astro_calc_minutes(tm) + td.astro;
  }

  postcond(timi->minutes[ASTRO_MINTS] == MINUTES_DISABLED
	   || timi->minutes[ASTRO_MINTS] < MINT_PER_DAY);

  return true;
}

bool 
get_timer_minutes(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard) {
  time_t timer = time(NULL);
  struct tm *tm = localtime(&timer);
  return get_timer_minutes_tm(timi, group, member, wildcard, tm);
}

minutes_t
timi_get_earliest(timer_minutes_t *timi, minutes_t now) {
  u8 i;
  minutes_t last = MINUTES_DISABLED;
  for (i=0; i < SIZE_MINTS; ++i) {
    if (timi->minutes[i] <= now)
      continue; // timer not in future

    last = MIN(last, timi->minutes[i]);
  }
  return last;
}


#define MAX_GRP 7
#define MAX_MBR 7
enum pass { PASS_GET_EARLIEST_TIME, PASS_FILL_EVENTS, PASS_STOP};

bool 
get_next_timer_event(timer_event_t *teu, timer_event_t *ted) {
  precond(teu && ted);
  u8 pass, g, m;
  minutes_t earliest = MINUTES_DISABLED;
  gm_bitmask_t existing_members = {0,};

  time_t timer = time(NULL);
  struct tm *tm = localtime(&timer);
  minutes_t minutes_now = tm->tm_hour * 60 + tm->tm_min;

  memset((void*)teu, 0, sizeof *teu);
  teu->next_event = MINUTES_DISABLED;
  SET_BIT(teu->flags, tef_UP);

  memset((void*)ted, 0, sizeof *ted);
  ted->next_event = MINUTES_DISABLED;
  SET_BIT(ted->flags, tef_DOWN);

  for (pass=0; pass < PASS_STOP; ++pass) {
    for (g=0; g <= MAX_GRP; ++g) {
      for (m=0; m <= MAX_MBR; ++m) {
	timer_minutes_t timi;

  if (gm_GetBit(&manual_bits, g, m)) {
    continue;
  }

	if (pass != PASS_GET_EARLIEST_TIME &&  !GET_BIT(existing_members[g], m)) {
	  // file not exists - copy parent and continue with next
	  if (g > 0 && m == 0) {
	    if (te_match(teu, 0, 0))
	      te_set_match(teu, g, 0); // copy parent g=0
	    if (te_match(ted, 0, 0))
	      te_set_match(ted, g, 0); // copy parent g=0
	  } else if (g > 0 && m > 0) {
	    if (te_match(teu, g, 0))
	      te_set_match(teu, g, m); // copy parent g=1..7 m=0
	    if (te_match(ted, g, 0))
	      te_set_match(ted, g, m); // copy parent g=1..7 m=0
	  }
	  continue;
	}

	if (get_timer_minutes_tm(&timi, &g, &m, false, tm)) {
	  SET_BIT(existing_members[g], m);
	  if (pass == PASS_GET_EARLIEST_TIME) {
	    minutes_t temp = timi_get_earliest(&timi, minutes_now);
	    earliest = MIN(earliest, temp);
	    //if (temp != MINUTES_DISABLED) ets_printf("g=%u, m=%u, temp=%u - earliest=%u\n", g, m, temp, earliest);
	  } else if (pass == PASS_FILL_EVENTS) {
	    if (earliest == MINUTES_DISABLED) {
	      return false;
	    }

	    if (timi.minutes[DAILY_UP_MINTS] == earliest
		|| timi.minutes[WEEKLY_UP_MINTS] == earliest) {
	      te_set_match(teu, g, m);
	      teu->next_event = earliest;
	    }
	    if (timi.minutes[ASTRO_MINTS] == earliest
		|| timi.minutes[DAILY_DOWN_MINTS] == earliest
		|| timi.minutes[WEEKLY_DOWN_MINTS] == earliest) {
	      te_set_match(ted, g, m);
	      ted->next_event = earliest;
	    }
	  }
	}
      }
    }
  }
  return true;
}
