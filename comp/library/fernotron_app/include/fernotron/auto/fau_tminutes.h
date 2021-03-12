/**
 * \file     fernotron/auto/fau_tminutes.h
 * \brief    Represent  when each kind of timer will be scheduled for a given day and g/m. Calculate the next event from it.
 */

#pragma once

#include "fernotron/types.h"
#include "fernotron_trx/timer_data.h"

#include <stdint.h>
#include <stdbool.h>
#include <time.h>


typedef uint16_t fer_au_minutesT;

#define MINUTES_DISABLED ((fer_au_minutesT)~0)  ///< Special not-existing-minute value to mark a timer as disabled

/**
 * \brief   Kind of timers.
 * \note    Will be used as array index.
 */
enum {
  FER_MINTS_ASTRO, FER_MINTS_DAILY_UP, FER_MINTS_DAILY_DOWN, FER_MINTS_WEEKLY_UP, FER_MINTS_WEEKLY_DOWN, FER_MINTS_size
};

/**
 * \brief   A minute-of-day for each kind of timer.
 */
typedef struct {
  /**
   * \brief   Array of minute-of-day for each kind of timer.
   */
  fer_au_minutesT minutes[FER_MINTS_size];
} Fer_TimerMinutes;

/**
 * \brief  Get the minute-of-day from a struct tm
 */
inline fer_au_minutesT fer_au_mintsFromTm(const struct tm *tm) { return (tm->tm_hour * 60 + tm->tm_min); }


/**
 * \brief Get the minute-of-day from the current time
 */
fer_au_minutesT fer_au_mintsNow();

/**
 * \brief            Get the minute-of-day from a time_t
 * \param timer      Pointer to a time_t
 * \return           Return the calculated minute-of-day
 */
fer_au_minutesT fer_au_mintsFromTime(const time_t *timer);


// return minute offsets of todays timer events in parameter *result
// offsets are minutes after 00:00 hour.
// offset == MINUTES_DISABLED means: no timer event exists for today

/**
 * \brief        Convert Fer_TimerData object to Fer_TimerMinutes object for the given point of time.
 * \param timi   Caller provided memory where the created object will be stored
 * \param tdp    Data to convert from
 * \param tm     The point of time.  This determines which will be the next event.  NULL for current time.
 * \return       false if an error occurred (will never happen)  XXX
 */
bool fer_au_get_timer_minutes_from_timer_data_tm(Fer_TimerMinutes *timi, const Fer_TimerData *tdp, const struct tm *tm = NULL);

/**
 * \brief        Get next event in Fer_TimerMinutes.
 * \param timi   Input data
 * \param now    Current time as minute-of-day
 * \return
 *         - The minute-of-day the next event will occur.
 *         - MINUTES_DISABLED: no event will happen until the end of day
 */
fer_au_minutesT fer_au_get_earliest_from_timer_minutes(Fer_TimerMinutes *timi, fer_au_minutesT now);

