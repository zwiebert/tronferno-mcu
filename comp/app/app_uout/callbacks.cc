
#include <misc/int_macros.h>
#include <cli/mutex.hh>
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


static void publish_pctChange_gmp_asJson(const so_arg_gmp_t gmp) {
  LockGuard lock(cli_mutex);

  if (sj_open_root_object("tfmcu")) {
    soMsg_pos_print_gmp(gmp, true);
    sj_close_root_object();
    uoApp_publish_pctChange_json(sj_get_json(), false);
  }
}

void publish_pctChange_gmp_asJson(const so_arg_gmp_t *gmp, size_t len) {
  LockGuard lock(cli_mutex);

  if (sj_open_root_object("tfmcu")) {
    soMsg_pos_print_gmpa(gmp, true);
    sj_close_root_object();
    uoApp_publish_pctChange_json(sj_get_json(), false);
  }
}

void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a[], size_t len) {
  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pct_change)
      continue;

    if (it.flags.fmt.json) {
      publish_pctChange_gmp_asJson(a, len);
      return;
    }

    if (it.flags.fmt.obj) {
      uo_flagsT flags;
      flags.fmt.obj = true;
      flags.evt.pct_change = true;
      for (int i = 0; i < len; ++i) {
        publish(it.cb, &a[i], flags);
      }
    }
  }
}


void uoApp_publish_pctChange_gmp(const so_arg_gmp_t a) {

  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pct_change)
      continue;

    if (it.flags.fmt.json) {
      publish_pctChange_gmp_asJson(a);
      return;
    }

    if (it.flags.fmt.obj) {
      uo_flagsT flags;
      flags.fmt.obj = true;
      flags.evt.pct_change = true;
      publish(it.cb, &a, flags);
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

void uoApp_publish_pctChange_json(const char *json, bool fragment) {

  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pct_change)
      continue;
    if (!it.flags.fmt.json)
      continue;

    uo_flagsT flags;
    flags.fmt.json = true;
    flags.evt.pct_change = true;

    if (fragment) {
      publish_fragJson(it.cb, json, flags);
    } else {
      publish(it.cb, json, flags);
    }
  }
}
