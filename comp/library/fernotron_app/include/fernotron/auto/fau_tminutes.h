#pragma once

#include "fernotron/types.h"
#include "fernotron/trx/timer_data.h"

#include <stdint.h>
#include <stdbool.h>
#include <time.h>


typedef uint16_t fer_au_minutesT;

#define MINUTES_DISABLED ((fer_au_minutesT)~0)

enum {
  FER_MINTS_ASTRO, FER_MINTS_DAILY_UP, FER_MINTS_DAILY_DOWN, FER_MINTS_WEEKLY_UP, FER_MINTS_WEEKLY_DOWN, FER_MINTS_size
};

typedef struct {
  fer_au_minutesT minutes[FER_MINTS_size];
} Fer_TimerMinutes;

#define fer_au_mintsFromTm(tm) (tm->tm_hour * 60 + tm->tm_min)
fer_au_minutesT fer_au_mintsNow();
fer_au_minutesT fer_au_mintsFromTime(const time_t *timer);

// return minute offsets of todays timer events in parameter *result
// offsets are minutes after 00:00 hour.
// offset == MINUTES_DISABLED means: no timer event exists for today
bool fer_au_get_timer_minutes_from_timer_data_tm(Fer_TimerMinutes *timi, const Fer_TimerData *tdp, const struct tm *tm);
bool fer_au_get_timer_minutes_now(Fer_TimerMinutes *result, uint8_t *group, uint8_t *member, bool wildcard);
bool fer_au_get_timer_minutes_tim(Fer_TimerMinutes *result, uint8_t *group, uint8_t *member, bool wildcard, const time_t *now_time);
bool fer_au_get_timer_minutes_tm(Fer_TimerMinutes *result, uint8_t *group, uint8_t *member, bool wildcard, const struct tm *tm);

// find earliest event in Fer_TimerMinutes
fer_au_minutesT fer_au_get_earliest_from_timer_minutes(Fer_TimerMinutes *timi, fer_au_minutesT now);

