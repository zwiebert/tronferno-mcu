
#include <misc/int_macros.h>
#include <cli/mutex.hh>
#include <txtio/txtio_mutex.hh>
#include <app/uout/so_msg.h>
#include <app/uout/so_types.h>
#include <uout/status_json.h>
#include <stdio.h>
#include <string.h>
#include <debug/dbg.h>

#define UOUT_PROTECTED
#include <uout/callbacks.h>
#include <app/uout/callbacks.h>


static void publish(uoCb_cbT cb, const void *ptr, uo_flagsT flags) {
  std::scoped_lock lock { cli_mutex, txtio_mutex };
  uoCb_msgT  msg { .cv_ptr = ptr, .flags = flags };
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

void uoApp_publish_pinChange(const so_arg_pch_t args) {
  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pin_change)
      continue;

    if (it.flags.fmt.obj) {
      uo_flagsT flags;
      flags.fmt.obj = true;
      flags.evt.pin_change = true;

      publish(it.cb, &args, flags);
    }

    if (it.flags.fmt.json) {
      uo_flagsT flags;
      flags.fmt.json = true;
      flags.evt.pin_change = true;

      char buf[64];
      snprintf(buf, sizeof buf, "{\"mcu\":{\"gpio%d\":%d}}", args.gpio_num, args.level);
      publish(it.cb, buf, flags);
    }

  }
}


static void publish_pctChange_gmp_asJson(uoCb_cbT cb, const so_arg_gmp_t a) {
  LockGuard lock(cli_mutex);

  if (sj_open_root_object("tfmcu")) {
    sj_add_object("pct");
    char buf[] = "00";
    buf[0] += a.g;
    buf[1] += a.m;
    sj_add_key_value_pair_d(buf, a.p);
    sj_close_object();
    sj_close_root_object();

    uo_flagsT flags;
    flags.fmt.json = true;
    flags.evt.pct_change = true;
    publish(cb, sj_get_json(), flags);
  }
}

void publish_pctChange_gmp_asJson(uoCb_cbT cb, const so_arg_gmp_t *a, size_t len) {
  LockGuard lock(cli_mutex);

  if (sj_open_root_object("tfmcu")) {
    int start = sj_add_object("pct");
    for (int i = 0; a[i].g <= 7; ++i) {
      char buf[] = "00";
      buf[0] += a[i].g;
      buf[1] += a[i].m;
      sj_add_key_value_pair_d(buf, a[i].p);
    }
    sj_close_object();
    sj_close_root_object();

    uo_flagsT flags;
    flags.fmt.json = true;
    flags.evt.pct_change = true;
    publish(cb, sj_get_json(), flags);
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

    if (it.flags.fmt.obj) {
      uo_flagsT flags;
      flags.fmt.obj = true;
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

    if (it.flags.fmt.obj) {
      uo_flagsT flags;
      flags.fmt.obj = true;
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

