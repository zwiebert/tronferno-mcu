#include "app/config/proj_app_cfg.h"

#include <string.h>
#include "fernotron/fer_msg_rx.h"
#include "fernotron/sep/set_endpos.h"
#include "fernotron/pos/shutter_pct.h"
#include "app/uout/status_output.h"
#include "app/uout/so_msg.h"
#include "app/uout/callbacks.h"
#include "uout/status_json.hh"
#include "app/config/proj_app_cfg.h"
#include "fernotron/auto/fau_tminutes.h"
#include "app/rtc.h"
#include "fernotron/fer_msg_attachment.h"
#include "misc/bcd.h"
#include "cli_imp.h"
#include "app/cli/cli_fer.h"
#include "app/opt_map.hh"
#include "fernotron/auto/fau_tdata_store.h"
#include "app/settings/config.h"
#include <fernotron/fer_main.h>

#include <stdlib.h>

static void print_timer(const struct TargetDesc &td, u8 g, u8 m, bool wildcard); //XXX

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

#define is_kt(k) (kt == otok:: k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)

int process_parmTimer(clpar p[], int len, const struct TargetDesc &td) {
  int i;
  bool f_disableWeekly = false, f_disableDaily = false, f_disableAstro = false, f_disableManu = false;
  bool f_enableManu = false;

  fer_grp group = fer_grp_Broadcast;
  fer_memb memb = fer_memb_Broadcast;
  u32 addr = cfg_getCuId();
  ;
  u8 fpr0_mask = 0;
  i8 flag_rtc_only = FLAG_NONE;
  time_t timer = time(NULL);
  bool f_modify = false;
  bool f_no_send = false;
  Fer_TimerData tdr;
  Fer_TimerData tda = fer_td_initializer;
  u8 mn = 0;
  u8 rs = 0;

  for (i = 1; i < len; ++i) {
    const char *key = p[i].key, *val = p[i].val;

    if (!key)
    return -1;

    otok kt = optMap_findToken(key);

    switch (kt) {
      case otok::weekly:
      NODEFAULT();
      STRLCPY(tda.weekly, val, sizeof(tda.weekly) - 1);
      break;
      case otok::daily:
      NODEFAULT();
      STRLCPY(tda.daily, val, sizeof(tda.daily) - 1);
      break;
      case otok::astro:
      tda.astro = val ? atoi(val) : 0;
      break;
      case otok::random: {
        int flag = asc2bool(val);
        fer_td_put_random(&tda, flag >= 0);
        if (flag >= 0) {
          SET_BIT(fpr0_mask, flag_Random);
        }
      }
      break;
      case otok::sun_auto: {
        int flag = asc2bool(val);
        fer_td_put_sun_auto(&tda, flag >= 0);
        if (flag >= 0) {
          SET_BIT(fpr0_mask, flag_SunAuto);
        }
      }
      break;
      case otok::rtc_only:
      flag_rtc_only = asc2bool(val);
      break;
      case otok::a: {
        u32 tmp = val ? strtol(val, NULL, 16) : 0;
        if (tmp)
        addr = tmp;
      }
      break;
      case otok::g:
      if (!asc2group(val, &group))
      return cli_replyFailure(td);
      break;
      case otok::m:
      if (!asc2memb(val, &memb))
      return cli_replyFailure(td);
      mn = memb ? (memb - 7) : 0;
      break;
      case otok::rtc: {
        time_t t = time_iso2time(val);
        if (t >= 0) {
          timer = t;
        }
      }
      break;
      case otok::rs: { // obsolete
        NODEFAULT();
        rs = atoi(val);
        f_no_send = true;
        f_modify = true;
      }
      break;

      case otok::f: {
        const char *p = val;
        NODEFAULT();
        while (*p) {
          switch (*p++) {
            case 'i':
            rs = 1;
            break;
            case 'I':
            rs = 2;
            break;
            case 'k':
            f_modify = true;
            break;
            case 'M':
            f_enableManu = true;
            break;
            case 'm':
            f_disableManu = true;
            break;
            case 'r':
            case 'R':
            SET_BIT(fpr0_mask, flag_Random);
            fer_td_put_random(&tda, p[-1] == 'R');
            break;
            case 's':
            case 'S':
            SET_BIT(fpr0_mask, flag_SunAuto);
            fer_td_put_sun_auto(&tda, p[-1] == 'S');
            break;
            case 'u':
            f_no_send = true;
            break;
            // disable timers and override any implicit enabling by value
            case 'd':
            f_disableDaily = true;
            break;
            case 'w':
            f_disableWeekly = true;
            break;
            case 'a':
            f_disableAstro = true;
            break;
          }
        }

      }
      break;

      default:
      cli_warning_optionUnknown(td, key);
      break;

    } //esac

  } //rof

  fer_sbT *fsb = fer_get_fsb(addr, group, mn, fer_cmd_Program);

  bool is_timer_frame = (FER_SB_ADDR_IS_CENTRAL(fsb) && flag_rtc_only != FLAG_TRUE);
  bool f_manual = false;

  if (is_timer_frame) {
    if (f_disableManu || f_enableManu) {
      manual_bits.putBit(group, mn, f_enableManu);
      fer_stor_gmBitMask_save("MANU", manual_bits, 1);
    }
    f_manual = manual_bits.getBit(group, mn);
  }

  bool need_reload_td,
  need_save_td;
  {
    bool f_modified =
        !f_modify
            || fer_td_is_astro(
                &tda) || f_disableAstro || fer_td_is_daily(&tda) || f_disableDaily || fer_td_is_weekly(&tda) || f_disableWeekly || GET_BIT(fpr0_mask, flag_Random) || GET_BIT(fpr0_mask, flag_SunAuto);
    bool no_read_save_td = (!f_modified && !f_disableManu) && !(f_modify && !f_no_send); // when not modified read/save would do nothing. but wee need to read when disabling manual mode

    need_reload_td = !no_read_save_td && is_timer_frame && f_modify; // load when modify existing data
    need_save_td = !no_read_save_td && is_timer_frame && f_modified; // save when data was modified
  }

  //db_printf("wasteful=%d, loading=%d, saving=%d\n modified=%d, modify=%d, manual=%d\n", no_read_save_td, need_reload_td, need_save_td, f_modified, f_modify, f_manual);

  // use (parts of) previously saved data
  if (need_reload_td) {
    u8 g = group, m = mn;
    // fill in missing parts from stored timer data
    if (fer_stor_timerData_load(&tdr, &g, &m, true)) {
      if (!f_disableDaily && !fer_td_is_daily(&tda) && fer_td_is_daily(&tdr)) {
        memcpy(tda.daily, tdr.daily, sizeof tdr.daily);
      }
      if (!f_disableWeekly && !fer_td_is_weekly(&tda) && fer_td_is_weekly(&tdr)) {
        memcpy(tda.weekly, tdr.weekly, sizeof tdr.weekly);
      }
      if (!f_disableAstro && !fer_td_is_astro(&tda) && fer_td_is_astro(&tdr)) {
        tda.astro = tdr.astro;
      }
      if (0 == GET_BIT(fpr0_mask, flag_Random)) {
        fer_td_put_random(&tda, fer_td_is_random(&tdr));
      }
      if (0 == GET_BIT(fpr0_mask, flag_SunAuto)) {
        fer_td_put_sun_auto(&tda, fer_td_is_sun_auto(&tdr));
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////

  if (!f_no_send) {
    if (flag_rtc_only == FLAG_TRUE) {
      cli_replyResult(td, fer_send_rtc_message(fsb, timer));
    } else if (f_manual) {
      cli_replyResult(td, fer_send_empty_timer_message(fsb, timer));
    } else {
      cli_replyResult(td, fer_send_timer_message(fsb, timer, &tda));
    }
  }

  // save timer data
  if (need_save_td) {
    if (fer_stor_timerData_save(&tda, group, mn)) {
      reply_message(td, "rs", "saved");
    } else {
      reply_message(td, "bug", "rs not saved");
      print_enr(td);
    }
  }

  if (rs) {
    if (int start = soMsg_timer_print_begin(td); start >= 0) {
      print_timer(td, group, mn, true);
      soMsg_timer_print_end(td);

      if (need_save_td) {
        const char *json = td.sj().get_json() + start;
        uoApp_publish_timer_json(json);
      }
    }
  }

  return 0;
}

static void print_timer(const struct TargetDesc &td, u8 g, u8 m, bool wildcard) {
  Fer_TimerData tdr;

  u8 g_res = g, m_res = m;

  if (fer_stor_timerData_load(&tdr, &g_res, &m_res, wildcard)) {
    soMsg_timer_begin(td, so_arg_gm_t { g, m });

    {
      bool f_manual = manual_bits.getBit(g, m);
      char flags[10], *p = flags;
      *p++ = f_manual ? 'M' : 'm';
      *p++ = fer_td_is_random(&tdr) ? 'R' : 'r';
      *p++ = fer_td_is_sun_auto(&tdr) ? 'S' : 's';
      *p++ = fer_td_is_astro(&tdr) ? 'A' : 'a';
      *p++ = fer_td_is_daily(&tdr) ? 'D' : 'd';
      *p++ = fer_td_is_weekly(&tdr) ? 'W' : 'w';
      *p++ = '\0';
      soMsg_kv(td, "f", flags);
    }

    if (fer_td_is_daily(&tdr)) {
      soMsg_kv(td, "daily", tdr.daily);
    }

    if (fer_td_is_weekly(&tdr)) {
      soMsg_kv(td, "weekly", tdr.weekly);
    }

    if (fer_td_is_astro(&tdr)) {
      soMsg_kv(td, "astro", tdr.astro);

      Fer_TimerMinutes tmi;
      fer_au_get_timer_minutes_now(&tmi, &g_res, &m_res, false);
      soMsg_kv(td, "asmin", tmi.minutes[FER_MINTS_ASTRO]);
    }

    soMsg_timer_end(td);
  }
}

