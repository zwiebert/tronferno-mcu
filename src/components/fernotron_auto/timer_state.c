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
get_timer_minutes_tm(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard, const struct tm *tm) {

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
get_timer_minutes_tim(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard, const time_t *now_time) {
  struct tm *tm = localtime(now_time);
  return get_timer_minutes_tm(timi, group, member, wildcard, tm);
}

bool
get_timer_minutes(timer_minutes_t *timi, u8 *group, u8 *member, bool wildcard) {
  time_t now_time = time(NULL);
  return get_timer_minutes_tim(timi, group, member, wildcard, &now_time);
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

static int set_earliest(u8 g, u8 m, minutes_t *earliest, const struct tm *tm_now, minutes_t minutes_now, gm_bitmask_t *gm) {
  int result = 0;

  timer_minutes_t timi;
  if (get_timer_minutes_tm(&timi, &g, &m, false, tm_now)) {
    minutes_t temp = timi_get_earliest(&timi, minutes_now);
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

static bool get_next_timer_event_earliest(gm_bitmask_t *mask_result, minutes_t *earliest_result, const struct tm *tm_now, minutes_t minutes_now) {
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

bool get_next_timer_event(timer_event_t *teu, timer_event_t *ted, const time_t *now_time) {
  precond(teu && ted);
  u8 g, m;
  minutes_t earliest = MINUTES_DISABLED;
  gm_bitmask_t existing_members = { 0, };

  *teu = (timer_event_t ) { .next_event = MINUTES_DISABLED, .flags = BIT(tef_UP) };
  *ted = (timer_event_t ) { .next_event = MINUTES_DISABLED, .flags = BIT(tef_DOWN) };


  const struct tm *tm_now = localtime(now_time);
  minutes_t minutes_now = tm_now->tm_hour * 60 + tm_now->tm_min;

  if (!get_next_timer_event_earliest(&existing_members, &earliest, tm_now, minutes_now))
    return false;


  for ((g = 0), (m = ~0); gm_getNext(&existing_members, &g, &m);) {
    if (gm_GetBit(&manual_bits, g, m))
      continue;

    timer_minutes_t timi;
    u8 g2 = g, m2 = m;
    if (!get_timer_minutes_tm(&timi, &g2, &m2, true, tm_now))
      continue; // should not happen

    minutes_t temp = timi_get_earliest(&timi, minutes_now);
    if (temp <= earliest) {

      if (timi.minutes[DAILY_UP_MINTS] == earliest || timi.minutes[WEEKLY_UP_MINTS] == earliest) {
        te_set_match(teu, g, m);
        teu->next_event = earliest;
      }
      if (timi.minutes[ASTRO_MINTS] == earliest || timi.minutes[DAILY_DOWN_MINTS] == earliest || timi.minutes[WEEKLY_DOWN_MINTS] == earliest) {
        te_set_match(ted, g, m);
        ted->next_event = earliest;
      }
    }
  }

  return true;
}


