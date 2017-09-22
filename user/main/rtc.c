#include "common.h"
#include "counter.h"
#include "utils.h"
#include "rtc.h"
#include <string.h>
#include <stdlib.h>
#ifdef AVR_TIME
#include <time.h>
#else
#include "../time/time.h"
#endif

void set_system_time(time_t timestamp);

void
rtc_setup()
{

}

int8_t ICACHE_FLASH_ATTR
get_weekDay()
{
  time_t timer = time(NULL);
  struct tm *tm = localtime(&timer);
  return tm->tm_wday;
}


// 0123456789012345678
//"YYYY-MM-DDThh:mm:ss"
bool ICACHE_FLASH_ATTR
rtc_get_by_string(char *s)
{
  time_t timer = time(NULL);
  struct tm *t = localtime(&timer);
  isotime_r(t, s);
  s[10] = 'T';
  return true;
}



bool ICACHE_FLASH_ATTR
rtc_set_by_string(const char *dateTimeString)
{
  if (strlen(dateTimeString) == 19) {
    char s[20], *token;
    
    strncpy(s, dateTimeString, 19);
    s[19] = '\0';
    struct tm t = {0, };
    
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

                  set_system_time(timestamp);
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


#if TEST_MODULE_RTC
bool testModule_rtc()
{
  char s[20]; bool result;
  result = rtc_set_by_string("2017-09-07T13:22:33");
  result = rtc_get_by_string(s);
  return result;
}

#endif
