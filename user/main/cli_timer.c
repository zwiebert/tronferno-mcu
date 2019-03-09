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
#include "timer_data.h"

#define ENABLE_RSTD 1

#define ENABLE_EXPERT 0 // work in progress
#define ENABLE_RESTART 1 // allow software reset

#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)


enum {
  TIMER_KEY_WEEKLY, TIMER_KEY_DAILY, TIMER_KEY_ASTRO, TIMER_KEY_RTC_ONLY, TIMER_KEY_FLAG_RANDOM, TIMER_KEY_FLAG_SUN_AUTO
};

const char * const timer_keys[] = { "weekly", "daily", "astro", "rtc-only", "random", "sun-auto" };

#define FLAG_NONE -2
#define FLAG_ERROR -1
#define FLAG_FALSE 0
#define FLAG_TRUE 1
#define HAS_FLAG(v) (v >= 0)

const char help_parmTimer[]  =
    "daily=T        set daily timer\n"
    "weekly=TTTTTTT set weekly timer\n"
    "astro[=N]      enables civil dusk timer +- minutes offset\n"
    "sun-auto       enables sun sensor\n"
    "random         enables random timer\n"
    "rtc-only       send no timers but update receiver's real time clock\n"
    "rtc=ISO_TIME   provide your own time\n"
    "a, g and m:    like in send command\n"
    "f=X...  flags (x|X means: x: enable, X: disable):\n"
    "  i   print saved timer information\n"
    "  I   like 'i' but returns matching group timers\n"
    "  k   keep and merge with existing saved timer\n"
    "  u   don't actually send data now\n"
    "  m|M manual mode (no automatic movement)\n"
    "  s|S sun automatic\n"
    "  r|R random timer\n"
    ;

int ICACHE_FLASH_ATTR
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
  time_t timer = time(NULL);
  bool f_modify = false;
  bool f_disableAuto = false;
  bool f_enableAuto = false;
  bool f_no_send = false;
  bool send_ok = false;
  char buf[16];

  static gm_bitmask_t manual_bits;

  const char *weeklyVal = 0, *dailyVal = 0;
  uint8_t mn = 0;
  uint8_t rs = 0;
  timer_data_t td = td_initializer;
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
      has_weekly = timerString2bcd((weeklyVal = val), weekly_data, sizeof weekly_data);
    } else if (strcmp(key, timer_keys[TIMER_KEY_DAILY]) == 0) {
      NODEFAULT();
      has_daily = timerString2bcd((dailyVal = val), daily_data, sizeof daily_data);
    } else if (strcmp(key, timer_keys[TIMER_KEY_ASTRO]) == 0) {
      astro_offset = val ? atoi(val) : 0;
      has_astro = true;
    } else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_RANDOM]) == 0) {
      int flag = asc2bool(val);
      if (flag >= 0) {
        SET_BIT(fpr0_mask, flag_Random);
        PUT_BIT(fpr0_flags, flag_Random, flag);
      }
    } else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_SUN_AUTO]) == 0) {
      int flag = asc2bool(val);
      if (flag >= 0) {
        SET_BIT(fpr0_mask, flag_SunAuto);
        PUT_BIT(fpr0_flags, flag_SunAuto, flag);
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

      mn = memb ? (memb - 7) : 0;
    } else if (strcmp(key, "rtc") == 0) {
      time_t t = time_iso2time(val);
      if (t >= 0) {
        timer = t;
      }
    } else if (strcmp(key, "rs") == 0) { // obsolete
      NODEFAULT();
      rs = atoi(val);
      f_no_send = true;
      f_modify = true;
    } else if (strcmp(key, "f") == 0) {
      const char *p = val;
      NODEFAULT();
      while (*p) {
        switch (*p++) {
          case 'i': rs = 1; break;
          case 'I': rs = 2; break;
          case 'k': f_modify = true; break;
          case 'M': f_disableAuto = true; break;
          case 'm': f_enableAuto = true; break;
          case 'r':
          case 'R': SET_BIT(fpr0_mask, flag_Random); PUT_BIT(fpr0_flags, flag_Random, p[-1] == 'R'); break;
          case 's':
          case 'S': SET_BIT(fpr0_mask, flag_SunAuto); PUT_BIT(fpr0_flags, flag_SunAuto, p[-1] == 'S'); break;
          case 'u': f_no_send = true; break;
        }
      }
#if ENABLE_TIMER_WDAY_KEYS
    } else if ((wday = asc2wday(key)) >= 0) {
      NODEFAULTS();
      io_puts(val), io_putlf();
      has_weekly = timerString2bcd(val, &weekly_data[FPR_TIMER_STAMP_WIDTH * wday], FPR_TIMER_STAMP_WIDTH);
#endif
    } else {
      if (strcmp(key, "rs") == 0) {
        reply_failure();
        return -1;
      }
      warning_unknown_option(key);
    }
  }

  bool is_timer_frame = (FSB_ADDR_IS_CENTRAL(fsb) && flag_rtc_only != FLAG_TRUE);
  bool f_manual = false;

  if (is_timer_frame) {
    read_gm_bitmask("MANU", &manual_bits, 1);
    if (f_enableAuto || f_disableAuto) {
      uint8_t g = group, m = mn;
      PUT_BIT(manual_bits[g], m, f_disableAuto);
      save_gm_bitmask("MANU", &manual_bits, 1);
    }
    f_manual = GET_BIT(manual_bits[group], mn);
  }


  // use (parts of) previously saved data
   if (f_modify || f_enableAuto) {
    uint8_t g = group, m = mn;

    // fill in missing parts from stored timer data
    if (read_timer_data(&td, &g, &m, true)) {
      if (!has_daily && td_is_daily(&td)) {
        has_daily = timerString2bcd(dailyVal = td.daily, daily_data, sizeof daily_data);
      }
      if (!has_weekly && td_is_weekly(&td)) {
        has_weekly = timerString2bcd(weeklyVal = td.weekly, weekly_data, sizeof weekly_data);
      }
      if (!has_astro && td_is_astro(&td)) {
        astro_offset = td.astro;
        has_astro = true;
      }
      if (0 == GET_BIT(fpr0_mask, flag_Random)) {
        PUT_BIT(fpr0_flags, flag_Random, td_is_random(&td));
        SET_BIT(fpr0_mask, flag_Random);
      }

      if (0 == GET_BIT(fpr0_mask, flag_SunAuto)) {
        PUT_BIT(fpr0_flags, flag_SunAuto, td_is_sun_auto(&td));
        SET_BIT(fpr0_mask, flag_SunAuto);
      }
    }
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////

  // create send buffer from all data

  // use fsb other than default
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

  // g, m
  if (FSB_ADDR_IS_CENTRAL(fsb)) {
    FSB_PUT_GRP(fsb, group);
    FSB_PUT_MEMB(fsb, memb);
  }

  FSB_PUT_CMD(fsb, fer_cmd_Program);
  FSB_TOGGLE(fsb);

  // copy data to send buffer
  if (recv_lockBuffer(true)) {
    fmsg_init_data(txmsg);
    if (flag_rtc_only == FLAG_TRUE) {
      fmsg_write_rtc(txmsg, timer, true);
      fmsg_write_flags(txmsg, fpr0_flags, fpr0_mask); // the flags are ignored for RTC-only frames, even for non-broadcast
    } else {
      if (has_weekly && !f_manual)
        fmsg_write_wtimer(txmsg, weekly_data);
      if (has_daily && !f_manual)
        fmsg_write_dtimer(txmsg, daily_data);
      if (has_astro && !f_manual)
        fmsg_write_astro(txmsg, astro_offset);
      fmsg_write_rtc(txmsg, timer, false);
      if (!f_manual)
        fmsg_write_flags(txmsg, fpr0_flags, fpr0_mask);
      fmsg_write_lastline(txmsg, fsb);
    }

    // send buffer
    if (!f_no_send) {
      send_ok = fer_send_msg(fsb, (flag_rtc_only == FLAG_TRUE) ? MSG_TYPE_RTC : MSG_TYPE_TIMER);
      reply(send_ok);
    }



    // save timer data
    if (is_timer_frame){  // FIXME: or better test for default cu?
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

    // print saved timer data
    if (rs) {
      uint8_t g = group, m = mn;

      cli_out_timer_reply_entry(NULL, NULL, 1);

      if (read_timer_data(&td, &g, &m, rs == 2)) {

        cli_out_timer_reply_entry("g", itoa(g, buf, 10), 0);
        cli_out_timer_reply_entry("m", itoa(m, buf, 10), 0);

        char *p = buf;
        *p++ = f_manual ? 'M' : 'm';
        *p++ = td_is_random(&td) ? 'R' : 'r';
        *p++ = td_is_sun_auto(&td) ? 'S' : 's';
        *p++ = td_is_astro(&td) ? 'A' : 'a';
        *p++ = td_is_daily(&td) ? 'D' : 'd';
        *p++ = td_is_weekly(&td) ? 'W' : 'w';
        *p++ = '\0';
        cli_out_timer_reply_entry("f", buf, 0);

        if (td_is_daily(&td)) {
          cli_out_timer_reply_entry("daily", td.daily, 0);
        }
        if (td_is_weekly(&td)) {
          cli_out_timer_reply_entry("weekly", td.weekly, 0);
        }
        if (td_is_astro(&td)) {
          cli_out_timer_reply_entry("astro", itoa(td.astro, buf, 10), 0);
        }

        if (td_is_random(&td)) {
          cli_out_timer_reply_entry("random", "1", 0);
        }
        if (td_is_sun_auto(&td)) {
          cli_out_timer_reply_entry("sun-auto", "1", 0);
        }

      }
      cli_out_timer_reply_entry(NULL, NULL, -1);
    }


    recv_lockBuffer(false);
  }
  return 0;
}
