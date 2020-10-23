#include "fernotron_uout/fer_uo_publish.h"

#include <uout/uo_callbacks.h>

// app
#include <fernotron_trx/fer_trx_api.hh>

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
    for (auto i = 0; i < idxs.size; ++i) {
      for (auto k = 0; k < len; ++k) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, &a[k], flags);
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
      for (int i = 0; a[i].g <= 7; ++i) {
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

      for (auto i = 0; i < idxs.size; ++i) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, sj.get_json(), flags);
      }
    }
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    for (auto k = 0; k < len; ++k) {
      snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;\n", a[k].g, a[k].m, a[k].p);  //XXX: Is inner or outer loop better?
      for (auto i = 0; i < idxs.size; ++i) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, &a[k], flags);
      }
    }
  }
}

void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a, uo_flagsT tgtFlags) {
  uo_flagsT flags;
  flags.evt.pct_change = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    for (auto i = 0; i < idxs.size; ++i) {
      uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, &a, flags);
    }
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

      for (auto i = 0; i < idxs.size; ++i) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, sj.get_json(), flags);
      }
    }
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;\n", a.g, a.m, a.p);
    for (auto i = 0; i < idxs.size; ++i) {
      uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, buf, flags);
    }
  }
}

void uoApp_publish_timer_json(const char *json, bool fragment) {
  uo_flagsT flags;
  flags.evt.timer_change = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {

    if (!fragment) {
      for (auto i = 0; i < idxs.size; ++i) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, json, flags);
      }
    } else {
      size_t json_len = strlen(json);
      char buf[json_len + 4] = "{";
      strcat(buf, json);
      buf[json_len] = '}'; // overwrite trailing comma
      buf[json_len + 1] = '\0';

      for (auto i = 0; i < idxs.size; ++i) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, buf, flags);
      }
    }
  }
}

void uoApp_publish_fer_msgReceived(const struct Fer_MsgPlainCmd *msg) {
  uo_flagsT flags;
  flags.evt.rf_msg_received = true;
  flags.evt.gen_app_state_change = true;

  const Fer_MsgPlainCmd &m = *msg;

  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];

    const char *cs = 0;
    const char *fdt = 0;

    if ((FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_PlainSender) && (fdt = "plain")) || (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit) && (fdt = "central"))) {
      switch (m.cmd) {
      case fer_if_cmd_DOWN:
        cs = "down";
        break;
      case fer_if_cmd_UP:
        cs = "up";
        break;
      case fer_if_cmd_STOP:
        cs = "stop";
        break;
      default:
        cs = 0;
        break;
      }
    } else if (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_SunSensor) && (fdt = "sun")) {
      switch (m.cmd) {
      case fer_if_cmd_SunDOWN:
        cs = "sun-down";
        break;
      case fer_if_cmd_SunUP:
        cs = "sun-up";
        break;
      case fer_if_cmd_SunINST:
        cs = "sun-pos";
        break;
      default:
        cs = 0;
        break;
      }
    }

    if (!cs)
      return; // unsupported command

    if (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit)) {
      snprintf(buf, sizeof buf, "RC:type=central: a=%06x g=%d m=%d c=%s;\n", m.a, m.g, m.m, cs);
    } else {
      snprintf(buf, sizeof buf, "RC:type=%s: a=%06x g=%d m=%d c=%s;\n", fdt, m.a, m.g, m.m, cs);
    }

    for (auto i = 0; i < idxs.size; ++i) {
      uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, buf, flags);
    }
  }
}

void uoApp_publish_fer_prasState(const so_arg_pras_t args) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_PRAS = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    for (auto i = 0; i < idxs.size; ++i) {
      uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, &args, flags);
    }
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

      for (auto i = 0; i < idxs.size; ++i) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, sj.get_json(), flags);
      }
    }
  }
}

void uoApp_publish_fer_cuasState(const so_arg_cuas_t args) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_CUAS = true;
  flags.evt.gen_app_state_change = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    for (auto i = 0; i < idxs.size; ++i) {
      uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, &args, flags);
    }
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

      for (auto i = 0; i < idxs.size; ++i) {
        uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, sj.get_json(), flags);
      }
    }
  }

  flags.fmt.json = false;
  flags.fmt.txt = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    const char *msg = args.success ? "tf:event:cuas=ok:;\n" : args.timeout ? "tf:event:cuas=time-out:;\n" : "tf:event:cuas=scanning:;\n";

    for (auto i = 0; i < idxs.size; ++i) {
      uoCb_publish(uoCb_cbs[idxs.arr[i]].cb, msg, flags);
    }
  }
}

