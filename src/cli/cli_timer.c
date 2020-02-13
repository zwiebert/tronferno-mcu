#include "user_config.h"

#include <string.h>

#include "../motor_setup/set_endpos.h"
#include "../shutter_positions/current_state.h"
#include "../userio/status_output.h"
#include "user_config.h"
#include "automatic/timer_state.h"
#include "main/rtc.h"
#include "fernotron/fer.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "automatic/timer_data.h"

#define ENABLE_RSTD 1

#define ENABLE_EXPERT 0 // work in progress
#define ENABLE_RESTART 1 // allow software reset

#define FSB_PLAIN_REPEATS 2  // send plain commands 1+N times (if 0, send only once without repeating)

gm_bitmask_t manual_bits;


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
    "f=X...  flags (x|X means: x: disable, X: enable):\n"
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
  bool f_disableWeekly = false, f_disableDaily = false, f_disableAstro = false, f_disableManu = false;
  bool f_enableWeekly = false, f_enableDaily = false, f_enableAstro = false, f_enableManu = false;

  i16 astro_offset = 0;
  u8 weekly_data[FPR_TIMER_STAMP_WIDTH * 7];
  u8 daily_data[FPR_TIMER_STAMP_WIDTH];
  fer_sender_basic *fsb = &default_sender;
  fer_grp group = fer_grp_Broadcast;
  fer_memb memb = fer_memb_Broadcast;
  u32 addr = 0;
  u8 fpr0_flags = 0, fpr0_mask = 0;
  i8 flag_rtc_only = FLAG_NONE;
  time_t timer = time(NULL);
  bool f_modify = false;
  bool f_no_send = false;
  bool send_ok = false;
  bool f_modified = false;
  timer_data_t tdr;

  const char *weeklyVal = 0, *dailyVal = 0;
  u8 mn = 0;
  u8 rs = 0;

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
      f_enableWeekly = timerString2bcd((weeklyVal = val), weekly_data, sizeof weekly_data);
    } else if (strcmp(key, timer_keys[TIMER_KEY_DAILY]) == 0) {
      NODEFAULT();
      f_enableDaily = timerString2bcd((dailyVal = val), daily_data, sizeof daily_data);
    } else if (strcmp(key, timer_keys[TIMER_KEY_ASTRO]) == 0) {
      astro_offset = val ? atoi(val) : 0;
      f_enableAstro = true;
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
          case 'M': f_enableManu = true; break;
          case 'm': f_disableManu = true; break;
          case 'r':
          case 'R': SET_BIT(fpr0_mask, flag_Random); PUT_BIT(fpr0_flags, flag_Random, p[-1] == 'R'); break;
          case 's':
          case 'S': SET_BIT(fpr0_mask, flag_SunAuto); PUT_BIT(fpr0_flags, flag_SunAuto, p[-1] == 'S'); break;
          case 'u': f_no_send = true; break;
          // disable timers and override any implicit enabling by value
          case 'd': f_disableDaily = true; break;
          case 'w': f_disableWeekly = true; break;
          case 'a': f_disableAstro = true; break;
        }
      }
    } else {
      if (strcmp(key, "rs") == 0) {
        reply_failure();
        return -1;
      }
      warning_unknown_option(key);
    }
  }

  bool is_timer_frame = (FSB_ADDR_IS_CENTRAL(fsb) && flag_rtc_only != FLAG_TRUE); // FIXME: better test for default cu?
  bool f_manual = false;

  if (is_timer_frame) {
    read_gm_bitmask("MANU", &manual_bits, 1);
    if (f_disableManu || f_enableManu) {
      PUT_BIT(manual_bits[group], mn, f_enableManu);
      save_gm_bitmask("MANU", &manual_bits, 1);
    }
    f_manual = GET_BIT(manual_bits[group], mn);
  }

  if (f_disableWeekly)
      f_enableWeekly = false;
  if (f_disableAstro)
      f_enableAstro =false;
  if (f_disableDaily)
    f_enableDaily = false;

  f_modified = !f_modify || f_enableAstro || f_disableAstro || f_enableDaily || f_disableDaily || f_enableWeekly || f_disableWeekly || GET_BIT(fpr0_mask, flag_Random) || GET_BIT(fpr0_mask, flag_SunAuto);

  bool no_read_save_td = (!f_modified && !f_disableManu) && !(f_modify && !f_no_send); // when not modified read/save would do nothing. but wee need to read when disabling manual mode
  bool need_reload_td = !no_read_save_td && is_timer_frame && f_modify; // load when modify existing data
  bool need_save_td   = !no_read_save_td && is_timer_frame && f_modified; // save when data was modified
  bool copy_automatics = !f_manual;  // in manual mode don't write some data to send-buffer

  //db_printf("wasteful=%d, loading=%d, saving=%d\n modified=%d, modify=%d, manual=%d\n", no_read_save_td, need_reload_td, need_save_td, f_modified, f_modify, f_manual);

  // use (parts of) previously saved data
   if (need_reload_td) {
    u8 g = group, m = mn;

    // fill in missing parts from stored timer data
    if (read_timer_data(&tdr, &g, &m, true)) {
      if (!f_disableDaily && !f_enableDaily && td_is_daily(&tdr)) {
        f_enableDaily = timerString2bcd(dailyVal = tdr.daily, daily_data, sizeof daily_data);
      }
      if (!f_disableWeekly && !f_enableWeekly && td_is_weekly(&tdr)) {
        f_enableWeekly = timerString2bcd(weeklyVal = tdr.weekly, weekly_data, sizeof weekly_data);
      }
      if (!f_disableAstro && !f_enableAstro && td_is_astro(&tdr)) {
        astro_offset = tdr.astro;
        f_enableAstro = true;
      }
      if (0 == GET_BIT(fpr0_mask, flag_Random)) {
        PUT_BIT(fpr0_flags, flag_Random, td_is_random(&tdr));
        SET_BIT(fpr0_mask, flag_Random);
      }

      if (0 == GET_BIT(fpr0_mask, flag_SunAuto)) {
        PUT_BIT(fpr0_flags, flag_SunAuto, td_is_sun_auto(&tdr));
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
  if (!f_no_send && recv_lockBuffer(true)) {
    fmsg_init_data(txmsg);
    if (flag_rtc_only == FLAG_TRUE) {
      fmsg_write_rtc(txmsg, timer, true);
      fmsg_write_flags(txmsg, fpr0_flags, fpr0_mask); // the flags are ignored for RTC-only frames, even for non-broadcast
    } else {
      if (copy_automatics) {
        if (!f_disableWeekly && f_enableWeekly)
          fmsg_write_wtimer(txmsg, weekly_data);
        if (!f_disableDaily && f_enableDaily)
          fmsg_write_dtimer(txmsg, daily_data);
        if (!f_disableAstro && f_enableAstro)
          fmsg_write_astro(txmsg, astro_offset);
        fmsg_write_rtc(txmsg, timer, false);
        fmsg_write_flags(txmsg, fpr0_flags, fpr0_mask);
      }
      fmsg_write_lastline(txmsg, fsb);
    }
    fsb->repeats = 0;
    // send buffer
    send_ok = fer_send_msg(fsb, (flag_rtc_only == FLAG_TRUE) ? MSG_TYPE_RTC : MSG_TYPE_TIMER);
    reply(send_ok);

    recv_lockBuffer(false);
  }

  // save timer data
  if (need_save_td) {
    timer_data_t tds = td_initializer;
    if (f_enableAstro) {
      tds.astro = astro_offset;
    }
    tds.bf = fpr0_flags;
    if (f_enableWeekly) {
      strncpy(tds.weekly, weeklyVal, sizeof(tds.weekly) - 1);
    }

    if (f_enableDaily) {
      strncpy(tds.daily, dailyVal, sizeof(tds.daily) - 1);
    }

    if (save_timer_data(&tds, group, mn)) {
      reply_message("rs", "saved");
    } else {
      reply_message("bug", "rs not saved");
      print_enr();
    }
  }

  if (rs) {
    so_arg_gm_t gm = {group, mn};
    so_output_message(SO_TIMER_PRINT, &gm);
  }


  return 0;
}


