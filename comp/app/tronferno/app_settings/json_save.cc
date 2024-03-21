#include "uout/so_target_desc.hh"
#include "fernotron/fer_main.h"

#include <string.h>
#include "fernotron/pos/shutter_pct.h"
#include "fernotron/alias/pairings.h"
#include "app_uout/status_output.h"
#include "app_uout/so_msg.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "uout/status_json.hh"

#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/fer_main.h"
#include "app_misc/rtc.h"
#include "app_misc/kvstore.hh"
#include "utils_misc/bcd.h"
#include "utils_misc/cstring_utils.h"
#include "app_misc/opt_map.hh"
#include "fernotron/auto/fau_tdata_store.h"
#include "app_settings/config.h"
#include <fernotron/fer_main.h>
#include <fernotron_trx/fer_trx_api.hh>

#include <fernotron/pos/shutter_prefs.h>

static void print_timer(const class UoutWriter &td, uint8_t g, uint8_t m, bool wildcard) {
  Fer_TimerData tdr;

  uint8_t g_res = g, m_res = m;

  if (fer_stor_timerData_load(&tdr, &g_res, &m_res, wildcard)) {
    if (td.sj().add_object()) {

      soMsg_kv(td, "g", g);
      soMsg_kv(td, "m", m);

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

      if (tdr.hasAstro() && tdr.getAstro() != 0) {
        soMsg_kv(td, "astro", tdr.getAstro());
      }

      td.sj().close_object();
    }
  }
}

static bool print_shpref(const class UoutWriter &td, uint8_t g, uint8_t m, bool hide_gm = false) {
  char name[64];
  struct shutter_timings timings;

  const bool has_name = fer_shPref_strByM_load(name, sizeof name, "NAME", g, m);
  const bool has_timings = m != 0 && fer_pos_prefByM_load(&timings, g, m);

  if (has_name || has_timings) {
    td.sj().add_object();

    if (!hide_gm) {
      soMsg_kv(td, "g", g);
      soMsg_kv(td, "m", m);
    }

    if (has_name)
      soMsg_kv(td, "tag.NAME", name);

    if (has_timings) {
      soMsg_kv(td, "mvut", timings.move_up_tsecs);
      soMsg_kv(td, "mvdt", timings.move_down_tsecs);
      soMsg_kv(td, "mvspdt", timings.move_sundown_tsecs);
    }

    td.sj().close_object();
    return true;
  }
  return false;
}

void appSett_all_settings_to_json(const UoutWriter &td) {
  auto gm_set = fer_usedMemberMask;
  gm_set.updateGroup();

  if (td.sj().add_object("settings")) {

    // config
    if (td.sj().add_object("config")) {
      soCfg_all(td, true);
      td.sj().close_object();
      td.sj().write_some_json();
    }

    // auto
    if (td.sj().add_array("auto")) {
      for (auto it = gm_set.begin(); it; ++it) {
        uint8_t g = it.getG(), m = it.getM();
        print_timer(td, g, m, false);
      }
      td.sj().close_array();
      td.sj().write_some_json();
    }

    // shpref
    if (td.sj().add_array("shpref")) {
      for (auto it = gm_set.begin(); it; ++it) {
        uint8_t g = it.getG(), m = it.getM();

        print_shpref(td, g, m, false);
      }
      td.sj().close_array();
      td.sj().write_some_json();
    }

    // pair
    if (td.sj().add_object("pair")) {
      fer_alias_so_output_all_pairings(td, true);
      td.sj().close_object();
      td.sj().write_some_json();
    }

    // kvs
    if (td.sj().add_object("kvs")) {
      kvs_foreach_string(csu_startsWith, "TXN.", [](const char *key, kvs_type_t type, const UoutWriter &td) -> kvs_cbrT {
        char value[80];
        if (type == KVS_TYPE_STR && kvs_get_string(key, value, sizeof(value))) {
          td.so().print(key, value);
          return kvsCb_match;
        }
        return kvsCb_noMatch;
      }, td);
      td.sj().close_object();
      td.sj().write_some_json();
    }

    td.sj().close_object(); // settings
  }
}
