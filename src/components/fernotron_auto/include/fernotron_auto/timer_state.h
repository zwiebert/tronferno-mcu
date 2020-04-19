#ifndef USER_MAIN_TIMER_STATE_H_
#define USER_MAIN_TIMER_STATE_H_

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "fernotron/types.h"


enum { ASTRO_MINTS,
       DAILY_UP_MINTS, DAILY_DOWN_MINTS,
       WEEKLY_UP_MINTS, WEEKLY_DOWN_MINTS,
       SIZE_MINTS
};

typedef uint16_t minutes_t;
#define MINUTES_DISABLED ((minutes_t)~0)
typedef struct {
  minutes_t minutes[SIZE_MINTS];
} timer_minutes_t;

#define tm_DMin(tm) (tm->tm_hour * 60 + tm->tm_min)
minutes_t get_now_min();
minutes_t get_min(time_t timer);

// return minute offsets of todays timer events
// offsets are minutes after 00:00 hour.
// offset == -1 means: no timer event exists for today
bool get_timer_minutes_now(timer_minutes_t *result, uint8_t *group, uint8_t *member, bool wildcard);
bool get_timer_minutes_tim(timer_minutes_t *result, uint8_t *group, uint8_t *member, bool wildcard, const time_t *now_time);
bool get_timer_minutes_tm(timer_minutes_t *result, uint8_t *group, uint8_t *member, bool wildcard, const struct tm *tm);

minutes_t timi_get_earliest(timer_minutes_t *timi, minutes_t now);




typedef uint8_t te_flags_t;
enum te_flags {tef_UP, tef_DOWN, };

#define te_is_up(te) GET_BIT((te)->flags, tef_UP)
#define te_is_down(te) GET_BIT((te)->flags, tef_DOWN)
#define te_match(te,g,m) GET_BIT((te)->matching_members[(g)], (m))
#define te_set_match(te,g,m) SET_BIT((te)->matching_members[(g)], (m))

typedef struct {
  gm_bitmask_t matching_members;
  minutes_t next_event;
  te_flags_t flags;

} timer_event_t;

bool get_next_timer_event(timer_event_t *up, timer_event_t *down, const time_t *now_time);


#endif
