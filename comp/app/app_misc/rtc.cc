#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_misc/rtc.h"

#include <stdlib.h>
#include <time.h>
#ifdef USE_POSIX_TIME
#include <sys/time.h>
#endif
#include <string.h>

// 0123456789012345678
//"YYYY-MM-DDThh:mm:ss"
bool 
rtc_get_by_string(char *s) {
  time_t timer = time(NULL);
  struct tm t;
  localtime_r(&timer, &t);
#ifdef USE_POSIX_TIME
  strftime(s, 20, "%FT%H:%M:%S", &t);
#else
  isotime_r(&t, s);
  s[10] = 'T';
#endif
  return true;

}

time_t  time_iso2time(const char *dateTimeString) {
  if (strlen(dateTimeString) == 19) {
    char s[20], *token;

    STRCPY(s, dateTimeString);
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
                return timestamp;
              }
            }
          }
        }
      }
    }
  }
  return -1;
}

bool 
rtc_set_by_string(const char *dateTimeString) {
  time_t timestamp = time_iso2time(dateTimeString);
  if (timestamp > 0) {
#ifdef USE_POSIX_TIME
    struct timeval tv = {.tv_sec = timestamp, .tv_usec = 0 };
    settimeofday(&tv, NULL);
#else
#error  "Not implemented"
#endif
    return true;
  }
  return false;
}

i16 
eu_dst(const time_t *timer, i32 * z) {
  u32 t = *timer;
  if ((u8) (t >> 24) >= 194)
    t -= 3029443200U;
  t = (t + 655513200) / 604800 * 28;
  if ((u16) (t % 1461) < 856)
    return 3600;
  else
    return 0;
}

i16 
always_dst(const time_t *timer, i32 * z) {
  return 3600;
}



void 
rtc_setup() {

#ifdef USE_POSIX_TIME
  char buf[64];
  setenv("TZ", config_read_tz(buf, sizeof buf), 1);
  tzset();

#else

  set_zone(ONE_HOUR * config_read_timezone()); //* C.timezone);

  switch (C.geo_dST) {

  case dstEU:
    set_dst(eu_dst);
    break;

  case dstAlways:
     set_dst(always_dst);
    break;

  case dstNone:
  default:
     set_dst(NULL);

    break;
  }
#endif
}


