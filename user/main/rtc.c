#include "common.h"
#include "counter.h"
#include "utils.h"
#include "rtc.h"
#include <string.h>
#include <stdlib.h>
#if defined AVR_TIME
#include <time.h>
#elif  defined MCU_ESP32
#include <time.h>
#define ONE_HOUR 3600
#define ONE_DAY 86400
#else
#include "time/time.h"
#endif

#include "config.h"
#include "common.h"

rtc_time_source_t rtc_last_time_source;

void set_system_time(time_t timestamp);

#if defined MCU_ESP32
volatile rtc_time_t rtc_clock;
#define set_system_time(clock) (rtc_clock=(clock))
#endif

void ICACHE_FLASH_ATTR rtc_set_system_time(rtc_time_t stamp, rtc_time_source_t source) {
  rtc_last_time_source = source;
#ifdef MCU_ESP32
//FIXME: not implemented yet

#else
  set_system_time(stamp);
#endif
}
rtc_time_t ICACHE_FLASH_ATTR rtc_time(void) {
  return time(NULL);
}


int8_t ICACHE_FLASH_ATTR
get_weekDay() {
  time_t timer = time(NULL);
  struct tm *tm = localtime(&timer);
  return tm->tm_wday;
}

// 0123456789012345678
//"YYYY-MM-DDThh:mm:ss"
bool ICACHE_FLASH_ATTR
rtc_get_by_string(char *s) {
  time_t timer = time(NULL);
  struct tm *t = localtime(&timer);
#ifdef MCU_ESP32
  strftime(s, 20, "%FT%H:%M:%S", t);
#else
  isotime_r(t, s);
  s[10] = 'T';
#endif
  return true;

}

bool ICACHE_FLASH_ATTR
rtc_set_by_string(const char *dateTimeString) {
  if (strlen(dateTimeString) == 19) {
    char s[20], *token;

    strncpy(s, dateTimeString, 19);
    s[19] = '\0';
    struct tm t = { 0, };

    if ((token = strtok(s, "-"))) {
      t.tm_year = atoi(token) - 1900;
      if ((token = strtok(NULL, "-"))) {
        t.tm_mon = atoi(token) - 1;
        if ((token = strtok(NULL, "T "))) {
          t.tm_mday = atoi(token);
          if ((token = strtok(NULL, ":"))) {
            t.tm_hour = atoi(token);
            if ((token = strtok(NULL, ":"))) {
              t.tm_min = atoi(token);
              if ((token = strtok(NULL, "\0"))) {
                t.tm_sec = atoi(token);
                t.tm_isdst = -1;
                time_t timestamp = mktime(&t);
                if (timestamp > 0) {

                  rtc_set_system_time(timestamp, RTC_SRC_CLI); //FIXME: use real rtc_src
                  return true;
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}

#ifdef AVR_TIME
//#include <util/eu_dst.h>
#if 0
#include <util/usa_dst.h>

#else // FIXME: DST
int16_t ICACHE_FLASH_ATTR
usa_dst(const time_t *timer, int32_t * z) {
  uint32_t t = *timer;
  if ((uint8_t) (t >> 24) >= 194)
  t -= 3029443200U;
  t = (t + 655513200) / 604800 * 28;
  if ((uint16_t) (t % 1461) < 856)
  return 3600;
  else
  return 0;
}
#endif

#endif

int16_t ICACHE_FLASH_ATTR
eu_dst(const time_t *timer, int32_t * z) {
  uint32_t t = *timer;
  if ((uint8_t) (t >> 24) >= 194)
    t -= 3029443200U;
  t = (t + 655513200) / 604800 * 28;
  if ((uint16_t) (t % 1461) < 856)
    return 3600;
  else
    return 0;
}

int16_t ICACHE_FLASH_ATTR
always_dst(const time_t *timer, int32_t * z) {
  return 3600;
}

time_t rtc_timezone_in_secs() {
  time_t utc, local;
  utc = time(&local);
  localtime(&local);
  return local - utc;
}

void ICACHE_FLASH_ATTR
rtc_setup() {
#ifdef MCU_ESP32
  setenv("TZ", "Europe/Berlin", 1); //FIXME: not implemented
  tzset();
#else
  set_zone(ONE_HOUR * C.geo_timezone); //* C.timezone);
 //obsolete: set_position(C.geo_latitude * ONE_DEGREE, C.geo_longitude * ONE_DEGREE);
#endif
  switch (C.geo_dST) {

  case dstEU:
#ifdef MCU_ESP32
    // FIXME: not implemented yet
#else
    set_dst(eu_dst);
#endif
    break;
#ifdef AVR_TIME
    case dstUS:
    set_dst(usa_dst);
    break;
#endif
  case dstAlways:
#ifdef MCU_ESP32
    // FIXME: not implemented yet
#else
     set_dst(always_dst);
#endif
    break;
  case dstNone:
  default:
#ifdef MCU_ESP32
    // FIXME: not implemented yet
#else
     set_dst(NULL);
#endif
    break;
  }
}

#if TEST_MODULE_RTC
bool testModule_rtc()
{
  char s[20]; bool result;
  result = rtc_set_by_string("2017-09-07T13:22:33");
  result = rtc_get_by_string(s);
  return result;
}

#endif
