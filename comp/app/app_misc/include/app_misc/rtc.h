#pragma once

#include <time.h>
#include "utils_time/run_time.h"


void rtc_secTick(void);

//YYYY-MM-DDThh-mm-ss
bool rtc_set_by_string(const char *dateTimeString);
bool rtc_get_by_string(char *dst);
time_t time_iso2time(const char *dateTimeString);

time_t rtc_timezone_in_secs();

int8_t get_weekDay(void);
int16_t get_yearDay(void);
int get_dst(void);
int16_t rtc_get_next_minute(void); // get next minute_of_day or -1 until next minute begins

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


typedef time_t rtc_time_t;
typedef enum { RTC_SRC_NONE, RTC_SRC_NTP, RTC_SRC_CLI } rtc_time_source_t;

bool ntp_set_system_time(void);

void rtc_set_system_time(rtc_time_t stamp, rtc_time_source_t source);
rtc_time_t rtc_time(void);

extern rtc_time_source_t rtc_last_time_source;


