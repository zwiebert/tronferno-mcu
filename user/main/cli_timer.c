#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../user_config.h"
#include "current_state.h"
#include "timer_state.h"

#include "all.h"
#include "set_endpos.h"
#include "misc/bcd.h"
#include "cli_imp.h"


#if defined  MCU_ESP8266 || defined MCU_ESP32
#include "timer_data.h"
#define ENABLE_RSTD 1
#else
#define ENABLE_RSTD 0
#endif

#define ENABLE_EXPERT 0 // work in progress
#define ENABLE_RESTART 1 // allow software reset
#define ENABLE_TIMER_WDAY_KEYS 0  // allow timer mon=T tue=T sun=T  additional to weekly=TTTTTTT  (a waste of resources)

#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)


enum {
  TIMER_KEY_WEEKLY, TIMER_KEY_DAILY, TIMER_KEY_ASTRO, TIMER_KEY_RTC_ONLY, TIMER_KEY_FLAG_RANDOM, TIMER_KEY_FLAG_SUN_AUTO
};

const char * const timer_keys[] = { "weekly", "daily", "astro", "rtc-only", "random", "sun-auto" };

#if ENABLE_TIMER_WDAY_KEYS
const char *const timer_wdays[] = {"mon", "tue", "wed", "thu", "fri", "sat", "sun"};

int asc2wday(const char *s) {
  int i;
  for (i=0; i < (sizeof (timer_wdays) / sizeof (timer_wdays[0])); ++i) {
    if (strcmp(s, timer_wdays[i]) == 0)
    return i;
  }
  return -1;
}
#endif


#define FLAG_NONE -2
#define FLAG_ERROR -1
#define FLAG_FALSE 0
#define FLAG_TRUE 1
#define HAS_FLAG(v) (v >= 0)

const char help_parmTimer[] PROGMEM =
    "daily=T        enables daily timer. T is up/down like 0730- or 07302000 or -2000  for up 07:30 and/or down 20:00\n"
    "weekly=TTTTTTT enables weekly timer. T like with 'daily' or '+' to copy the T on the left. (weekly=0730-++++0900-+)\n"
    "astro[=N]      enables astro automatic. N is the offset to civil dusk in minutes. Can be postive or negative.\n"
    "sun-auto       enables sun automatic\n"
    "random         enables random automatic. shutter opens and closes at random times.\n"
    "rtc-only       don't change timers. only update internal real time clock\n"
    "a, g and m:    like in send command\n"
    "rs=(0|1|2)     read back saved timer data. if set to 2, return any data matching g and m e.g. m=0 (any member) instead of m=2\n";

/*static*/ int ICACHE_FLASH_ATTR
process_parmTimer(clpar p[], int len) {
  int i;
  bool has_daily = false, has_weekly = false, has_astro = false;
  int16_t astro_offset = 0;
  uint8_t weekly_data[FPR_TIMER_STAMP_WIDTH * 7];
  uint8_t daily_data[FPR_TIMER_STAMP_WIDTH];
  fer_sender_basic *fsb = &default_sender;
  fer_grp group = fer_grp_Broadcast;
  fer_memb memb = fer_memb_Broadcast;
  uint32_t addr = 0;
  uint8_t fpr0_flags = 0, fpr0_mask = 0;
  int8_t flag_rtc_only = FLAG_NONE;

#if ENABLE_RSTD
  const char *weeklyVal = 0, *dailyVal = 0;
  uint8_t mn = 0;
  uint8_t rs = 0;
  timer_data_t td = td_initializer;
#endif
  // init data
  for (i = 0; i + 1 < sizeof(weekly_data) / sizeof(weekly_data[0]); i += 2) {
    weekly_data[i] = 0xff;
    weekly_data[i + 1] = 0x0f;
  }
  for (i = 0; i + 1 < sizeof(daily_data) / sizeof(daily_data[0]); i += 2) {
    daily_data[i] = 0xff;
    daily_data[i + 1] = 0x0f;
  }

  for (i = 1; i < len; ++i) {
    const char *key = p[i].key, *val = p[i].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, timer_keys[TIMER_KEY_WEEKLY]) == 0) {
      NODEFAULT();
      has_weekly = string2bcdArray(val, weekly_data, sizeof weekly_data);
#if ENABLE_RSTD
      weeklyVal = val;
#endif
    } else if (strcmp(key, timer_keys[TIMER_KEY_DAILY]) == 0) {
      NODEFAULT();
      has_daily = string2bcdArray(val, daily_data, sizeof daily_data);
#if ENABLE_RSTD
      dailyVal = val;
#endif
    } else if (strcmp(key, timer_keys[TIMER_KEY_ASTRO]) == 0) {
      astro_offset = val ? atoi(val) : 0;
      has_astro = true;
    } else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_RANDOM]) == 0) {
      int flag = asc2bool(val);
      if (flag >= 0) {
        fpr0_mask |= (1 << flag_Random);
        if (flag)
        fpr0_flags |= (1 << flag_Random);
        else
        fpr0_flags &= ~(1 << flag_Random);
      }
    } else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_SUN_AUTO]) == 0) {
      int flag = asc2bool(val);
      if (flag >= 0) {
        fpr0_mask |= (1 << flag_SunAuto);
        if (flag)
        fpr0_flags |= (1 << flag_SunAuto);
        else
        fpr0_flags &= ~(1 << flag_SunAuto);
      }

    } else if (strcmp(key, timer_keys[TIMER_KEY_RTC_ONLY]) == 0) {
      flag_rtc_only = asc2bool(val);
    } else if (strcmp(key, "a") == 0) {
      addr = val ? strtol(val, NULL, 16) : 0;
    } else if (strcmp(key, "g") == 0) {
      if (!asc2group(val, &group))
      return reply_failure();
    } else if (strcmp(key, "m") == 0) {
      if (!asc2memb(val, &memb))
      return reply_failure();
#if ENABLE_RSTD
      mn = memb ? (memb - 7) : 0;
    } else if (strcmp(key, "rs") == 0) {
      NODEFAULT();
      rs = atoi(val);
#endif
#if ENABLE_TIMER_WDAY_KEYS
    } else if ((wday = asc2wday(key)) >= 0) {
      NODEFAULTS();
      io_puts(val), io_putlf();
      has_weekly = string2bcdArray(val, &weekly_data[FPR_TIMER_STAMP_WIDTH * wday], FPR_TIMER_STAMP_WIDTH);
#endif
    } else {
      if (strcmp(key, "rs") == 0) {
        reply_failure();
        return -1;
      }
      warning_unknown_option(key);
    }
  }

#if ENABLE_RSTD
  if (rs) {
    uint8_t g = group, m = mn;

    reply_print("rs=data");
    if (read_timer_data(&td, &g, &m, rs == 2)) {
      io_puts("timer");
      io_puts(" g="), io_putd(g);
      io_puts(" m="), io_putd(m);
      if (td_is_daily(&td)) {
        io_puts(" daily="), io_puts(td.daily);
      }
      if (td_is_weekly(&td)) {
        io_puts(" weekly="), io_puts(td.weekly);
      }
      if (td_is_astro(&td)) {
        io_puts(" astro="), io_putd(td.astro);
      }
      if (td_is_random(&td)) {
        io_puts(" random=1");
      }
      if (td_is_sun_auto(&td)) {
        io_puts(" sun-auto=1");
      }

      io_puts(";\n");
      reply_success();
    } else {
      io_puts("none\n");
    }
    return 0;
  }
#endif

  if (addr != 0) {
    fsb = get_sender_by_addr(addr);
    if (!fsb) {
      static fer_sender_basic fsb_direct; // FIXME: or was senders[0] meant for this?
      fsb = &fsb_direct;
      if (FSB_GET_DEVID(fsb) != addr) {
        fer_init_sender(fsb, addr);
      }
    }
  }

  if (FSB_ADDR_IS_CENTRAL(fsb)) {
    FSB_PUT_GRP(fsb, group);
    FSB_PUT_MEMB(fsb, memb);
  }

  FSB_PUT_CMD(fsb, fer_cmd_Program);
  FSB_TOGGLE(fsb);

  if (recv_lockBuffer(true)) {
    fmsg_init_data(txmsg);
    if (flag_rtc_only == FLAG_TRUE) {
      fmsg_write_rtc(txmsg, true);
      fmsg_write_flags(txmsg, fpr0_flags, fpr0_mask); // the flags are ignored for RTC-only frames, even for non-broadcast
    } else {
      if (has_weekly)
        fmsg_write_wtimer(txmsg, weekly_data);
      if (has_daily)
        fmsg_write_dtimer(txmsg, daily_data);
      if (has_astro)
        fmsg_write_astro(txmsg, astro_offset);
      fmsg_write_rtc(txmsg, false);
      fmsg_write_flags(txmsg, fpr0_flags, fpr0_mask);
      fmsg_write_lastline(txmsg, fsb);
    }

    if (reply(fer_send_msg(fsb, (flag_rtc_only == FLAG_TRUE) ? MSG_TYPE_RTC : MSG_TYPE_TIMER))) {
#if ENABLE_RSTD
      if (FSB_ADDR_IS_CENTRAL(fsb) && flag_rtc_only != FLAG_TRUE) {  // FIXME: or better test for default cu?
        if (has_astro) {
          td.astro = astro_offset;
        }
        td.bf = fpr0_flags;
        if (has_weekly) {
          strncpy(td.weekly, weeklyVal, sizeof(td.weekly) - 1);
        }

        if (has_daily) {
          strncpy(td.daily, dailyVal, sizeof(td.daily) - 1);
        }

        if (save_timer_data(&td, group, mn)) {
          reply_message("rs", "saved");
        } else {
          reply_message("bug", "rs not saved");
          print_enr();
        }
      }
#endif
    }

    recv_lockBuffer(false);
  }
  return 0;
}
