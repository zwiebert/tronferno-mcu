#pragma once

#include "fernotron/types.h"
#include "fernotron/timer_data.h"

#include <stdint.h>
#include <stdbool.h>
#include <time.h>


typedef uint16_t minutes_t;

#define MINUTES_DISABLED ((minutes_t)~0)

enum {
  ASTRO_MINTS, DAILY_UP_MINTS, DAILY_DOWN_MINTS, WEEKLY_UP_MINTS, WEEKLY_DOWN_MINTS, SIZE_MINTS
};

typedef struct {
  minutes_t minutes[SIZE_MINTS];
} Fer_TimerMinutes;

#define tm_DMin(tm) (tm->tm_hour * 60 + tm->tm_min)
minutes_t get_now_min();
minutes_t get_min(const time_t *timer);

// return minute offsets of todays timer events in parameter *result
// offsets are minutes after 00:00 hour.
// offset == MINUTES_DISABLED means: no timer event exists for today
bool fer_au_get_timer_minutes_from_timer_data_tm(Fer_TimerMinutes *timi, const Fer_TimerData *tdp, const struct tm *tm);
bool fer_au_get_timer_minutes_now(Fer_TimerMinutes *result, uint8_t *group, uint8_t *member, bool wildcard);
bool fer_au_get_timer_minutes_tim(Fer_TimerMinutes *result, uint8_t *group, uint8_t *member, bool wildcard, const time_t *now_time);
bool fer_au_get_timer_minutes_tm(Fer_TimerMinutes *result, uint8_t *group, uint8_t *member, bool wildcard, const struct tm *tm);

// find earliest event in Fer_TimerMinutes
minutes_t fer_au_get_earliest_from_timer_minutes(Fer_TimerMinutes *timi, minutes_t now);

