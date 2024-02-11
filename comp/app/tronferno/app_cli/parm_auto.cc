

#include <string.h>
#include "fernotron/pos/shutter_pct.h"
#include "app_uout/status_output.h"
#include "app_uout/so_msg.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "uout/status_json.hh"

#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/fer_main.h"
#include "app_misc/rtc.h"
#include "utils_misc/bcd.h"
#include "cli_internal.hh"
#include "cli_fernotron.hh"
#include "app_misc/opt_map.hh"
#include "fernotron/auto/fau_tdata_store.h"
#include "app_settings/config.h"
#include <fernotron/fer_main.h>
#include <fernotron_trx/fer_trx_api.hh>

#include <stdlib.h>

static void print_timer(const class UoutWriter &td, uint8_t g, uint8_t m, bool wildcard); //XXX

#define FLAG_NONE -2
#define FLAG_ERROR -1
#define FLAG_FALSE 0
#define FLAG_TRUE 1
#define HAS_FLAG(v) (v >= 0)
#define is_val(k) (strcmp(val, k) == 0)

const char cli_help_parmTimer[] = "'auto' programs the built-in automatic movement functionality\n\n"
    "a=(0|ID)  0  programming center ID.\n"
    "g=[0-7]   0  group number\n"
    "m=[0-7]   0  receiver number\n"
    "daily=T        set daily timer\n"
    "weekly=TTTTTTT set weekly timer\n"
    "astro[=N]      enables civil dusk timer +- minutes offset\n"
    "sun-auto=(1|0) 0 enables sun sensor\n"
    "random=(1|0)   0 enables random timer offset (0...30 min)\n"
    "rtc-only=(1|0) 0 send no timers but update receiver's real time clock\n"
    "rtc=ISO_TIME   provide your own time\n"
    "f=X...  flags (x|X means: x: disable, X: enable):\n"
    "  i   print saved timer information\n"
    "  I   like 'i' but returns matching group timers\n"
    "  k   keep and merge with existing saved timer\n"
    "  u   don't actually send data now\n"
    "  m|M manual mode (no automatic movement)\n"
    "  s|S sun automatic\n"
    "  r|R random timer\n";

int process_parmTimer(clpar p[], int len, const class UoutWriter &td) {
  int i;
  bool f_disableWeekly = false, f_disableDaily = false, f_disableAstro = false, f_disableManu = false;
  bool f_enableManu = false;
  uint8_t parm_g = 0, parm_m = 0;
  uint32_t addr = fer_config.cu;
  bool has_parm_Random = false, has_parm_SunAuto = false;
  int8_t flag_rtc_only = FLAG_NONE;
  time_t timer = time(NULL);
  bool f_modify = false;
  bool f_no_send = false;
  Fer_TimerData tdr;
  Fer_TimerData tda;
  uint8_t rs = 0; // ==1: return timer data (like f=i).  ==2 return wildcard timer data (like f=I)

  for (i = 1; i < len; ++i) {
    const char *key = p[i].key, *val = p[i].val;

    if (!key)
    return -1;

    otok kt = optMap_findToken(key);

    switch (kt) {
      case otok::k_weekly:
      NODEFAULT();
      if (!tda.putWeekly(val)) {
        return cli_replyFailure(td);
      }
      break;
      case otok::k_daily:
      NODEFAULT();
      if (!tda.putDaily(val)) {
        return cli_replyFailure(td);
      }
      break;
      case otok::k_astro:
      tda.putAstro(val ? atoi(val) : 0);
      break;
      case otok::k_random: {
        int flag = asc2bool(val);
        tda.putRandom( flag >= 0);
        if (flag >= 0) {
          has_parm_Random = true;
        }
      }
      break;
      case otok::k_sun_auto: {
        int flag = asc2bool(val);
        tda.putSunAuto( flag >= 0);
        if (flag >= 0) {
          has_parm_SunAuto = true;
        }
      }
      break;
      case otok::k_rtc_only:
      flag_rtc_only = asc2bool(val);
      break;
      case otok::k_a: {
        uint32_t tmp = val ? strtol(val, NULL, 16) : 0;
        if (tmp)
          addr = tmp;
        else if (!is_val("0"))
          return cli_replyFailure(td);
      }
      break;
      case otok::k_g: {
        if (!asc2u8(val, &parm_g, 7))
        return cli_replyFailure(td);
      }
      break;
      case otok::k_m: {
      if (!asc2u8(val, &parm_m, 7))
      return cli_replyFailure(td);
      }
      break;
      case otok::k_rtc: {
        time_t t = time_iso2time(val);
        if (t >= 0) {
          timer = t;
        }
      }
      break;

      case otok::k_f: {
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
            has_parm_Random = true;
            tda.putRandom( p[-1] == 'R');
            break;
            case 's':
            case 'S':
            has_parm_SunAuto = true;
            tda.putSunAuto( p[-1] == 'S');
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


  bool is_timer_frame = FER_U32_TEST_TYPE(addr, FER_ADDR_TYPE_CentralUnit) && flag_rtc_only != FLAG_TRUE;
  bool f_manual = false;
  bool f_manual_old = manual_bits.getMember(parm_g, parm_m);
  bool f_manual_changed = false;

  if (is_timer_frame) {
    if (f_disableManu || f_enableManu) {
      manual_bits.putMember(parm_g, parm_m, f_enableManu);
      manual_bits.store_save(MANUAL_BITS_STORE_NAME);
    }
    f_manual = manual_bits.getMember(parm_g, parm_m);
    f_manual_changed = f_manual != f_manual_old;
  }

  bool need_reload_td,
  need_save_td;
  {
    bool f_modified =
        !f_modify
            ||
                tda.hasAstro() || f_disableAstro || tda.hasDaily() || f_disableDaily || tda.hasWeekly() || f_disableWeekly || has_parm_Random || has_parm_SunAuto;
    bool no_read_save_td = (!f_modified && !f_disableManu) && !(f_modify && !f_no_send); // when not modified read/save would do nothing. but wee need to read when disabling manual mode

    need_reload_td = !no_read_save_td && is_timer_frame && f_modify; // load when modify existing data
    need_save_td = !no_read_save_td && is_timer_frame && f_modified; // save when data was modified
  }

  //db_printf("wasteful=%d, loading=%d, saving=%d\n modified=%d, modify=%d, manual=%d\n", no_read_save_td, need_reload_td, need_save_td, f_modified, f_modify, f_manual);

  // use (parts of) previously saved data
  if (need_reload_td) {
    uint8_t g = parm_g, m = parm_m;
    // fill in missing parts from stored timer data
    if (fer_stor_timerData_load(&tdr, &g, &m, true)) {
      if (!f_disableDaily && !tda.hasDaily() && tdr.hasDaily()) {
        tda.putDaily(tdr.getDaily());
      }
      if (!f_disableWeekly && !tda.hasWeekly() && tdr.hasWeekly()) {
        tda.putWeekly(tdr.getWeekly());
      }
      if (!f_disableAstro && !tda.hasAstro() && tdr.hasAstro()) {
        tda.putAstro(tdr.getAstro());
      }
      if (!has_parm_Random) {
        tda.putRandom(tdr.getRandom());
      }
      if (!has_parm_SunAuto) {
        tda.putSunAuto(tdr.getSunAuto());
      }
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////

  if (!f_no_send) {
    if (flag_rtc_only == FLAG_TRUE) {
      Fer_MsgRtc msg { .a = addr, .g = parm_g, .m = parm_m, .rtc = timer };
      cli_replyResult(td, Fer_Trx_API::send_rtc(msg));
    } else if (f_manual) {
      Fer_MsgRtc msg { .a = addr, .g = parm_g, .m = parm_m, .rtc = timer };
      cli_replyResult(td, Fer_Trx_API::send_empty_timer(msg));
    } else {
      Fer_MsgTimer msg { .a = addr, .g = parm_g, .m = parm_m, .rtc = timer, .td = &tda };
      cli_replyResult(td, Fer_Trx_API::send_timer(msg));
    }
  }

  // save timer data
  if (need_save_td) {
    if (fer_stor_timerData_save(&tda, parm_g, parm_m)) {
      reply_message(td, "rs", "saved");
      uoApp_publish_timer_json(parm_g, parm_m, &tda);
    } else {
      reply_message(td, "bug", "rs not saved");
      print_enr(td);
    }
  } else if (f_manual_changed) {
    Fer_TimerData tdb;
    uint8_t rg = parm_g, rm = parm_m;
    if (fer_stor_timerData_load(&tdb, &rg, &rm, true)) {
      uoApp_publish_timer_json(rg, rm, &tda);
    }
  }

  if (rs) {
    if (int start = soMsg_timer_print_begin(td, p[0].key); start >= 0) {
      print_timer(td, parm_g, parm_m, rs == 2);
      soMsg_timer_print_end(td);
    }
  }

  return 0;
}

static void print_timer(const class UoutWriter &td, uint8_t g, uint8_t m, bool wildcard) {
  Fer_TimerData tdr;

  uint8_t g_res = g, m_res = m;

  if (fer_stor_timerData_load(&tdr, &g_res, &m_res, wildcard)) {
    soMsg_timer_begin(td, so_arg_gm_t { g, m });

    {
      bool f_manual = manual_bits.getMember(g, m);
      char flags[10], *p = flags;
      *p++ = f_manual ? 'M' : 'm';
      *p++ = tdr.getRandom() ? 'R' : 'r';
      *p++ = tdr.getSunAuto() ? 'S' : 's';
      *p++ = tdr.hasAstro() ? 'A' : 'a';
      *p++ = tdr.hasDaily() ? 'D' : 'd';
      *p++ = tdr.hasWeekly() ? 'W' : 'w';
      *p++ = '\0';
      soMsg_kv(td, "f", flags);
    }

    if (tdr.hasDaily()) {
      soMsg_kv(td, "daily", tdr.getDaily());
    }

    if (tdr.hasWeekly()) {
      soMsg_kv(td, "weekly", tdr.getWeekly());
    }

    if (tdr.hasAstro()) {
      soMsg_kv(td, "astro", tdr.getAstro());

      Fer_TimerMinutes tmi;
      fer_au_get_timer_minutes_from_timer_data_tm(&tmi, &tdr);
      soMsg_kv(td, "asmin", tmi.minutes[FER_MINTS_ASTRO]);
    }

    soMsg_timer_end(td);
  }
}

