/**
 * \file    app_misc/rtc.h
 * \brief   Some time related functions and constants.
 *
 * XXX: Refactor this
 */

#pragma once

#include <time.h>
#include "utils_time/run_time.h"

/**
 * \brief                    Set current time by ISO string
 * \param  dateTimeString    ISO format time string: YYYY-MM-DDThh-mm-ss
 */
bool rtc_set_by_string(const char *dateTimeString);

/**
 * \brief           Get current time as ISO string
 * \param[out] dst  Pointer to buffer to store the ISO time string
 * \return          true for success
 */
bool rtc_get_by_string(char *dst);

/**
 * \brief                   Get POSIX time value from ISO string
 * \param dateTimeString    ISO format time string: YYYY-MM-DDThh-mm-ss
 * \return                  POSIX time
 */
time_t time_iso2time(const char *dateTimeString);

/// \brief  Set up this module
void rtc_setup(void);

#define MSEC_PER_SEC 1000
#define MINT_PER_HOUR  60
#define MINT_PER_DAY  (60 * 24)
#define SECS_PER_MINT  60
#define HOUR_PER_DAY   24
#define MONT_PER_YEAR  12
#define DAYS_PER_WEEK   7
#define SECS_PER_DAY 86400
#define SECS_PER_HOUR (60 * 60)



/**
 * \brief           store current state of tm_isdst
 * \param  reinit   if false, do nothing on subsequent calls
 */
void dst_init(struct tm *tmp = nullptr, bool reinit = false);

/**
 * \brief check if tm_isdst has changed since last call to dst_init
 * \return  true if tm_isdst has changed. If true, you should call dst_init(true) to update the internal state
 */
bool dst_hasChanged(struct tm *tmp = nullptr);



