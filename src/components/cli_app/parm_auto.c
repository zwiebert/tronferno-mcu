#include "app_config/proj_app_cfg.h"

#include <string.h>
#include "fernotron/fer_msg_rx.h"
#include "fernotron_sep/set_endpos.h"
#include "fernotron_pos/shutter_pct.h"
#include "userio_app/status_output.h"
#include "app_config/proj_app_cfg.h"
#include "fernotron_auto/timer_state.h"
#include "app/rtc.h"
#include "fernotron/fer_msg_attachment.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "cli_fer.h"
#include "fernotron_auto/timer_data.h"
#include "config/config.h"

gm_bitmask_t manual_bits;

enum {
  TIMER_KEY_WEEKLY, TIMER_KEY_DAILY, TIMER_KEY_ASTRO, TIMER_KEY_RTC_ONLY, TIMER_KEY_FLAG_RANDOM, TIMER_KEY_FLAG_SUN_AUTO
};

const char *const timer_keys[] = { "weekly", "daily", "astro", "rtc-only", "random", "sun-auto" };

#define FLAG_NONE -2
#define FLAG_ERROR -1
#define FLAG_FALSE 0
#define FLAG_TRUE 1
#define HAS_FLAG(v) (v >= 0)

const char cli_help_parmTimer[] = "daily=T        set daily timer\n"
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
    "  r|R random timer\n";

int process_parmTimer(clpar p[], int len) {
  int i;
  bool f_disableWeekly = false, f_disableDaily = false, f_disableAstro = false, f_disableManu = false;
  bool f_enableManu = false;

  fer_grp group = fer_grp_Broadcast;
  fer_memb memb = fer_memb_Broadcast;
  u32 addr = C.fer_centralUnitID;
  ;
  u8 fpr0_mask = 0;
  i8 flag_rtc_only = FLAG_NONE;
  time_t timer = time(NULL);
  bool f_modify = false;
  bool f_no_send = false;
  timer_data_t tdr;
  timer_data_t tda = td_initializer;
  u8 mn = 0;
  u8 rs = 0;

  for (i = 1; i < len; ++i) {
    const char *key = p[i].key, *val = p[i].val;

    if (key == NULL) {
      return -1;
    } else if (strcmp(key, timer_keys[TIMER_KEY_WEEKLY]) == 0) {
      NODEFAULT();
      strncpy(tda.weekly, val, sizeof(tda.weekly) - 1);
    } else if (strcmp(key, timer_keys[TIMER_KEY_DAILY]) == 0) {
      NODEFAULT();
      strncpy(tda.daily, val, sizeof(tda.daily) - 1);
    } else if (strcmp(key, timer_keys[TIMER_KEY_ASTRO]) == 0) {
      tda.astro = val ? atoi(val) : 0;
    } else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_RANDOM]) == 0) {
      int flag = asc2bool(val);
      td_put_random(&tda, flag >= 0);
      if (flag >= 0) {
        SET_BIT(fpr0_mask, flag_Random);
      }
    } else if (strcmp(key, timer_keys[TIMER_KEY_FLAG_SUN_AUTO]) == 0) {
      int flag = asc2bool(val);
      td_put_sun_auto(&tda, flag >= 0);
      if (flag >= 0) {
        SET_BIT(fpr0_mask, flag_SunAuto);
      }

    } else if (strcmp(key, timer_keys[TIMER_KEY_RTC_ONLY]) == 0) {
      flag_rtc_only = asc2bool(val);
    } else if (strcmp(key, "a") == 0) {
      u32 tmp = val ? strtol(val, NULL, 16) : 0;
      if (tmp) addr = tmp;
    } else if (strcmp(key, "g") == 0) {
      if (!asc2group(val, &group))
      return cli_replyFailure();
    } else if (strcmp(key, "m") == 0) {
      if (!asc2memb(val, &memb))
      return cli_replyFailure();

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
            case 'R': SET_BIT(fpr0_mask, flag_Random); td_put_random(&tda, p[-1] == 'R'); break;
            case 's':
            case 'S': SET_BIT(fpr0_mask, flag_SunAuto); td_put_sun_auto(&tda, p[-1] == 'S'); break;
            case 'u': f_no_send = true; break;
            // disable timers and override any implicit enabling by value
            case 'd': f_disableDaily = true; break;
            case 'w': f_disableWeekly = true; break;
            case 'a': f_disableAstro = true; break;
          }
        }
      } else {
        if (strcmp(key, "rs") == 0) {
          cli_replyFailure();
          return -1;
        }
        cli_warning_optionUnknown(key);
      }
    }

    fsbT *fsb = get_fsb(addr, group, mn, fer_cmd_Program);

    bool is_timer_frame = (FSB_ADDR_IS_CENTRAL(fsb) && flag_rtc_only != FLAG_TRUE);
    bool f_manual = false;

    if (is_timer_frame) {
      fer_gmByName_load("MANU", &manual_bits, 1);
      if (f_disableManu || f_enableManu) {
        PUT_BIT(manual_bits[group], mn, f_enableManu);
        fer_gmByName_store("MANU", &manual_bits, 1);
      }
      f_manual = GET_BIT(manual_bits[group], mn);
    }

    bool need_reload_td,
  need_save_td;
  {
    bool f_modified =
        !f_modify
            || td_is_astro(
                &tda) || f_disableAstro || td_is_daily(&tda) || f_disableDaily || td_is_weekly(&tda) || f_disableWeekly || GET_BIT(fpr0_mask, flag_Random) || GET_BIT(fpr0_mask, flag_SunAuto);
    bool no_read_save_td = (!f_modified && !f_disableManu) && !(f_modify && !f_no_send); // when not modified read/save would do nothing. but wee need to read when disabling manual mode

    need_reload_td = !no_read_save_td && is_timer_frame && f_modify; // load when modify existing data
    need_save_td = !no_read_save_td && is_timer_frame && f_modified; // save when data was modified
  }

  //db_printf("wasteful=%d, loading=%d, saving=%d\n modified=%d, modify=%d, manual=%d\n", no_read_save_td, need_reload_td, need_save_td, f_modified, f_modify, f_manual);

  // use (parts of) previously saved data
  if (need_reload_td) {
    u8 g = group, m = mn;
    // fill in missing parts from stored timer data
    if (read_timer_data(&tdr, &g, &m, true)) {
      if (!f_disableDaily && !td_is_daily(&tda) && td_is_daily(&tdr)) {
        memcpy(tda.daily, tdr.daily, sizeof tdr.daily);
      }
      if (!f_disableWeekly && !td_is_weekly(&tda) && td_is_weekly(&tdr)) {
        memcpy(tda.weekly, tdr.weekly, sizeof tdr.weekly);
      }
      if (!f_disableAstro && !td_is_astro(&tda) && td_is_astro(&tdr)) {
        tda.astro = tdr.astro;
      }
      if (0 == GET_BIT(fpr0_mask, flag_Random)) {
        td_put_random(&tda, td_is_random(&tdr));
      }
      if (0 == GET_BIT(fpr0_mask, flag_SunAuto)) {
        td_put_sun_auto(&tda, td_is_sun_auto(&tdr));
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////

  if (!f_no_send) {
    if (flag_rtc_only == FLAG_TRUE) {
      cli_replyResult(send_rtc_message(fsb, timer));
    } else if (f_manual) {
      cli_replyResult(send_empty_timer_message(fsb, timer));
    } else {
      cli_replyResult(send_timer_message(fsb, timer, &tda));
    }
  }

  // save timer data
  if (need_save_td) {
    if (save_timer_data(&tda, group, mn)) {
      reply_message("rs", "saved");
    } else {
      reply_message("bug", "rs not saved");
      print_enr();
    }
  }

  if (rs) {
    so_arg_gm_t gm = { group, mn };
    so_output_message(SO_TIMER_PRINT, &gm);
  }

  return 0;
}

