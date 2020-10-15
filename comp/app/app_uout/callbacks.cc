
#include <utils_misc/int_macros.h>
#include <utils_misc/cstring_utils.hh>
#include <cli/mutex.hh>
#include <txtio/txtio_mutex.hh>
#include <app_uout/so_msg.h>
#include <app_uout/so_types.h>
#include <uout/status_json.hh>
#include <fernotron_trx/fer_trx_api.hh>
#include <stdio.h>
#include <string.h>
#include <debug/dbg.h>

#define UOUT_PROTECTED
#include <uout/uo_callbacks.h>
#include <app_uout/callbacks.h>


static void publish(uoCb_cbT cb, const void *ptr, uo_flagsT flags) {
  std::scoped_lock lock { cli_mutex, txtio_mutex };
  uoCb_msgT  msg { .cptr = ptr, .flags = flags };
  cb(msg);
}
static void publish_fragJson(uoCb_cbT cb, const char *json, uo_flagsT flags) {
  size_t json_len = strlen(json);
  char buf[json_len + 4] = "{";
  strcat(buf, json);
  buf[json_len] = '}'; // overwrite trailing comma
  buf[json_len + 1] = '\0';
  publish(cb, buf, flags);
}




static void publish_pctChange_gmp_asJson(uoCb_cbT cb, const so_arg_gmp_t a) {
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

    uo_flagsT flags;
    flags.fmt.json = true;
    flags.evt.pct_change = true;
    publish(cb, sj.get_json(), flags);
  }
}

void publish_pctChange_gmp_asJson(uoCb_cbT cb, const so_arg_gmp_t *a, size_t len) {
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
    publish(cb, sj.get_json(), flags);
  }
}

static void publish_pctChange_gmp_asTxt(uoCb_cbT cb, const so_arg_gmp_t a) {
  char buf[64];
  snprintf(buf, sizeof buf, "A:position: g=%d m=%d p=%d;\n", a.g, a.m, a.p);
  uo_flagsT flags;
  flags.fmt.txt = true;
  flags.evt.pct_change = true;
  publish(cb, buf, flags);

}

void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a[], size_t len, uo_flagsT tgtFlags) {
  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pct_change)
      continue;

    if (it.flags.fmt.json) {
      publish_pctChange_gmp_asJson(it.cb, a, len);
    }

    if (it.flags.fmt.raw) {
      uo_flagsT flags;
      flags.fmt.raw = true;
      flags.evt.pct_change = true;
      for (int i = 0; i < len; ++i) {
        publish(it.cb, &a[i], flags);
      }
    }

    if (it.flags.fmt.txt) {
      for (int i = 0; i < len; ++i) {
        publish_pctChange_gmp_asTxt(it.cb, a[i]);
      }
    }
  }
}

void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a, uo_flagsT tgtFlags) {

  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pct_change)
      continue;

    if (it.flags.fmt.json) {
      publish_pctChange_gmp_asJson(it.cb, a);
    }

    if (it.flags.fmt.raw) {
      uo_flagsT flags;
      flags.fmt.raw = true;
      flags.evt.pct_change = true;
      publish(it.cb, &a, flags);
    }

    if (it.flags.fmt.txt) {
      publish_pctChange_gmp_asTxt(it.cb, a);
    }
  }
}

void uoApp_publish_timer_json(const char *json, bool fragment) {
  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.timer_change)
      continue;
    if (!it.flags.fmt.json)
      continue;

    uo_flagsT flags;
    flags.fmt.json = true;
    flags.evt.timer_change = true;

    if (fragment) {
      publish_fragJson(it.cb, json, flags);
    } else {
      publish(it.cb, json, flags);
    }
  }
}

static void publish_fer_msgReceived_asTxt(uoCb_cbT cb, const struct Fer_MsgPlainCmd &m) {
  char buf[64];
  fer_if_cmd c = m.cmd;
  u32 id = m.a;

  const char *cs = 0;
  const char *fdt = 0;

  if ((FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_PlainSender) && (fdt = "plain")) || (FER_U32_TEST_TYPE(m.a, FER_ADDR_TYPE_CentralUnit) && (fdt = "central"))) {
    switch (c) {
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
    switch (c) {
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
  uo_flagsT flags;
  flags.fmt.txt = true;
  flags.evt.rf_msg_received = true;
  publish(cb, buf, flags);

}

void uoApp_publish_fer_msgReceived(const struct Fer_MsgPlainCmd *msg) {
  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.rf_msg_received)
      continue;
    //TODO: add json output for this (RF msg received)

    if (it.flags.fmt.txt) {
      publish_fer_msgReceived_asTxt(it.cb, *msg);
    }
  }

}

