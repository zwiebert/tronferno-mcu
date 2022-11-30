#include "fernotron_uout/fer_uo_publish.h"

#include <uout/uo_callbacks.h>

// app
#include <fernotron_trx/fer_trx_api.hh>
#include <fernotron_trx/timer_data.h>
#include <fernotron/fer_main.h>
#include <fernotron/auto/fau_tminutes.h>
#include <app_uout/so_msg.h>

// shared
#include <utils_misc/int_macros.h>
#include <utils_misc/cstring_utils.hh>
#include <uout/status_json.hh>
#include <debug/dbg.h>

// sys
#include <stdio.h>
#include <string.h>

void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a[], size_t len, uo_flagsT tgtFlags) {
  uo_flagsT flags;
  flags.evt.pct_change = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    {
      for (auto i = 0; i < len; ++i) {
        uoCb_publish(idxs, &a[i], flags);
      }
    }
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char sj_buf[128];
    StatusJsonT sj = { sj_buf, sizeof sj_buf };

    if (sj.open_root_object("tfmcu")) {
      sj.add_object("pct");
      for (int i = 0; i < len; ++i) {
        char buf[] = "00";
        buf[0] += a[i].g;
        buf[1] += a[i].m;
        sj.add_key_value_pair_d(buf, a[i].p);
      }
      sj.close_object();
      sj.close_root_object();

      uo_flagsT flags;
      flags.fmt.json = true;
      flags.evt.pct_change = true;

      uoCb_publish(idxs, sj.get_json(), flags);
    }
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    for (auto i = 0; i < len; ++i) {
      snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;", a[i].g, a[i].m, a[i].p);
      uoCb_publish(idxs, buf, flags);
    }
  }
}

void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a, uo_flagsT tgtFlags) {
  uo_flagsT flags;
  flags.evt.pct_change = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    uoCb_publish(idxs, &a, flags);
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char sj_buf[128];
    StatusJsonT sj = { sj_buf, sizeof sj_buf };

    if (sj.open_root_object("tfmcu")) {
      sj.add_object("pct");
      char buf[] = "00";
      buf[0] += a.g;
      buf[1] += a.m;
      sj.add_key_value_pair_d(buf, a.p);
      sj.close_object();
      sj.close_root_object();

      uoCb_publish(idxs, sj.get_json(), flags);
    }
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;", a.g, a.m, a.p);
    uoCb_publish(idxs, buf, flags);
  }
}

void uoApp_publish_timer_json(const char *json, bool fragment) {
  uo_flagsT flags;
  flags.evt.timer_change = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {

    if (!fragment) {
      uoCb_publish(idxs, json, flags);
    } else {
      size_t json_len = strlen(json);
      char buf[json_len + 4] = "{";
      strcat(buf, json);
      buf[json_len] = '}'; // overwrite trailing comma
      buf[json_len + 1] = '\0';

      uoCb_publish(idxs, buf, flags);
    }
  }
}

static void timer_json(TargetDesc &td, uint8_t g, uint8_t m, struct Fer_TimerData &tdr) {
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

void uoApp_publish_timer_json(uint8_t g, uint8_t m, struct Fer_TimerData *tda) {
  uo_flagsT flags;
  flags.evt.timer_change = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    TargetDesc td { SO_TGT_HTTP | SO_TGT_FLAG_JSON };
    td.sj().open_root_object("tfmcu.publish");
    td.sj().add_object("auto");
    timer_json(td, g, m, *tda);
    td.sj().close_object();
    td.sj().close_root_object();

    uoCb_publish(idxs, td.sj().get_json(), flags);
  }
}

struct cmdInfo {
  const char *cs = 0;
  const char *fdt = 0;
};
static cmdInfo cmdString_fromPlainCmd(const Fer_MsgPlainCmd &m) {
  struct cmdInfo r;

  if ((FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_PlainSender) && (r.fdt = "plain")) || (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit) && (r.fdt = "central"))
      || (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_Receiver) && (r.fdt = "receiver"))) {
    switch (m.cmd) {
    case fer_if_cmd_DOWN:
      r.cs = "down";
      break;
    case fer_if_cmd_UP:
      r.cs = "up";
      break;
    case fer_if_cmd_STOP:
      r.cs = "stop";
      break;
    case fer_if_cmd_SunDOWN:
      r.cs = "sun-down";
      break;
    case fer_if_cmd_SunUP:
      r.cs = "sun-up";
      break;
    case fer_if_cmd_SunINST:
      r.cs = "sun-pos";
      break;
    case fer_if_cmd_EndPosDOWN:
      r.cs = "sep-down";
      break;
    case fer_if_cmd_EndPosUP:
      r.cs = "sep-up";
      break;
    case fer_if_cmd_ToggleRotationDirection:
      r.cs = "rot-dir";
      break;
    default:
      r.cs = 0;
      break;
    }
  } else if (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_SunSensor) && (r.fdt = "sun")) {
    switch (m.cmd) {
    case fer_if_cmd_SunDOWN:
      r.cs = "sun-down";
      break;
    case fer_if_cmd_SunUP:
      r.cs = "sun-up";
      break;
    case fer_if_cmd_SunINST:
      r.cs = "sun-pos";
      break;
    case fer_if_cmd_Program:
      r.cs = "sun-test";
      break;
    default:
      r.cs = 0;
      break;
    }
  }
  return r;
}

void uoApp_publish_fer_msgSent(const struct Fer_MsgTimer *msg) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_msgSent = true;
  flags.evt.gen_app_state_change = true;

  const Fer_MsgTimer &m = *msg;
  if (!FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit))
    return;

  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[80];

    snprintf(buf, sizeof buf, "SA:type=central: a=%06x g=%d m=%d;", m.a, m.g, m.m);
    uoCb_publish(idxs, buf, flags);
  }

  flags.fmt.txt = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[80];

    snprintf(buf, sizeof buf, "{\"sa\":{\"type\":\"central\",\"a\":\"%06x\",\"g\":%d,\"m\":%d}}", m.a, m.g, m.m);
    uoCb_publish(idxs, buf, flags);
  }
}

void uoApp_publish_fer_msgSent(const struct Fer_MsgRtc *msg) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_msgSent = true;
  flags.evt.gen_app_state_change = true;

  const Fer_MsgRtc &m = *msg;
  if (!FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit))
    return;

  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[80];

    snprintf(buf, sizeof buf, "ST:type=central: a=%06x g=%d m=%d;", m.a, m.g, m.m);
    uoCb_publish(idxs, buf, flags);
  }

  flags.fmt.txt = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[80];

    snprintf(buf, sizeof buf, "{\"st\":{\"type\":\"central\",\"a\":\"%06x\",\"g\":%d,\"m\":%d}}", m.a, m.g, m.m);
    uoCb_publish(idxs, buf, flags);
  }
}


void uoApp_publish_fer_msgSent(const struct Fer_MsgPlainCmd *msg) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_msgSent = true;
  flags.evt.gen_app_state_change = true;

  const Fer_MsgPlainCmd &m = *msg;
  cmdInfo ci = cmdString_fromPlainCmd(m);
  if (!ci.cs)
    return; // unsupported command

  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[80];
    if (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit)) {
      snprintf(buf, sizeof buf, "SC:type=central: a=%06x g=%d m=%d c=%s;", m.a, m.g, m.m, ci.cs);
    } else {
      snprintf(buf, sizeof buf, "SC:type=%s: a=%06x g=%d m=%d c=%s;", ci.fdt, m.a, m.g, m.m, ci.cs);
    }
    uoCb_publish(idxs, buf, flags);
  }

  flags.fmt.txt = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[80];
    if (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit)) {
      snprintf(buf, sizeof buf, "{\"sc\":{\"type\":\"central\",\"a\":\"%06x\",\"g\":%d,\"m\":%d,\"c\":\"%s\"}}", m.a, m.g, m.m, ci.cs);
    } else {
      snprintf(buf, sizeof buf, "{\"sc\":{\"type\":\"%s\",\"a\":\"%06x\",\"c\":\"%s\"}}", ci.fdt, m.a, ci.cs);
    }
    uoCb_publish(idxs, buf, flags);
  }
}

void uoApp_publish_fer_msgReceived(const struct Fer_MsgPlainCmd *msg) {
  uo_flagsT flags;
  flags.evt.rf_msg_received = true;
  flags.evt.gen_app_state_change = true;

  const int rssi = Fer_Trx_API::get_rssi();

  const Fer_MsgPlainCmd &m = *msg;
  cmdInfo ci = cmdString_fromPlainCmd(m);
  if (!ci.cs)
    return; // unsupported command

  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    if (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit)) {
      snprintf(buf, sizeof buf, "RC:type=central: a=%06x g=%d m=%d c=%s rssi=%d;", m.a, m.g, m.m, ci.cs, rssi);
    } else {
      snprintf(buf, sizeof buf, "RC:type=%s: a=%06x g=%d m=%d c=%s rssi=%d;", ci.fdt, m.a, m.g, m.m, ci.cs, rssi);
    }
    uoCb_publish(idxs, buf, flags);
  }

  flags.fmt.txt = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[128];
    if (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit)) {
      snprintf(buf, sizeof buf, "{\"rc\":{\"type\":\"central\",\"a\":\"%06x\",\"g\":%d,\"m\":%d,\"c\":\"%s\",\"rssi\":%d}}", m.a, m.g, m.m, ci.cs, rssi);
    } else {
      snprintf(buf, sizeof buf, "{\"rc\":{\"type\":\"%s\",\"a\":\"%06x\",\"c\":\"%s\",\"rssi\":%d}}", ci.fdt, m.a, ci.cs, rssi);
    }
    uoCb_publish(idxs, buf, flags);
  }
}

void uoApp_publish_fer_prasState(const so_arg_pras_t args) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_PRAS = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    uoCb_publish(idxs, &args, flags);
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char a_hex[20];
    ltoa(args.a, a_hex, 16);
    char sj_buf[128];
    StatusJsonT sj = { sj_buf, sizeof sj_buf };

    if (sj.open_root_object("tfmcu")) {

      sj.add_object("pras");
      sj.add_key_value_pair_s("a", a_hex);
      sj.add_key_value_pair_d("scanning", args.scanning);
      sj.add_key_value_pair_d("success", args.success);
      sj.add_key_value_pair_d("timeout", args.timeout);
      sj.add_key_value_pair_d("pairing", args.pairing);
      sj.close_object();

      sj.close_root_object();

      uoCb_publish(idxs, sj.get_json(), flags);
    }
  }
}

void uoApp_publish_fer_cuasState(const so_arg_cuas_t args) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_CUAS = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    uoCb_publish(idxs, &args, flags);
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char a_hex[20];
    ltoa(args.a, a_hex, 16);

    char sj_buf[128];
    StatusJsonT sj = { sj_buf, sizeof sj_buf };

    if (sj.open_root_object("tfmcu")) {
      sj.add_object("cuas");
      sj.add_key_value_pair_s("a", a_hex);
      sj.add_key_value_pair_d("scanning", args.scanning);
      sj.add_key_value_pair_d("success", args.success);
      sj.add_key_value_pair_d("timeout", args.timeout);
      sj.close_object();

      int state = args.scanning ? 1 : args.timeout ? 2 : args.success ? 3 : 0;
      sj.add_object("config");
      sj.add_key_value_pair_d("cuas", state);
      sj.close_object();

      sj.close_root_object();

      uoCb_publish(idxs, sj.get_json(), flags);
    }
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    const char *msg = args.success ? "tf:event:cuas=ok:;" : args.timeout ? "tf:event:cuas=time-out:;" : "tf:event:cuas=scanning:;";

    uoCb_publish(idxs, msg, flags);
  }
}

void uoApp_publish_fer_sepState(const so_arg_sep_t args, char tag) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_SEP = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    uoCb_publish(idxs, &args, flags);
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {

    char sj_buf[128];
    StatusJsonT sj = { sj_buf, sizeof sj_buf };

    if (sj.open_root_object("tfmcu")) {
      sj.add_object("sep");
      if (args.auth_success)
        sj.add_key_value_pair_d("auth-success", 1);
      if (args.auth_terminated)
        sj.add_key_value_pair_d("auth-terminated", 1);
      if (args.auth_button_timeout)
        sj.add_key_value_pair_d("auth-button-timeout", 1);
      if (args.auth_button_error)
        sj.add_key_value_pair_d("auth-button-error", 1);
      if (args.auth_timeout)
        sj.add_key_value_pair_d("auth-timeout", 1);
      if (args.ui_timeout)
        sj.add_key_value_pair_d("ui-timeout", 1);
      sj.close_object();

      sj.close_root_object();

      uoCb_publish(idxs, sj.get_json(), flags);
    }
  }
}
