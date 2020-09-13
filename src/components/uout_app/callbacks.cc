
#include <misc/int_macros.h>
#include <stdio.h>

#define UOUT_PROTECTED
#include <uout/callbacks.h>
#include <uout_app/callbacks.h>


static void publish(uoCb_cbT cb, const void *ptr, uo_flagsT flags) {
  uoCb_msgT  msg { .cv_ptr = ptr, .flags = flags };
  cb(msg);
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

void uoApp_publish_gmpJson(const char *json) {
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

    publish(it.cb, json, flags);
  }
}

void uoApp_publish_gmpObj(const so_arg_gmp_t a) {

  for (auto const &it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pct_change)
      continue;
    if (!it.flags.fmt.obj)
      continue;

    uo_flagsT flags;
    flags.fmt.obj = true;
    flags.evt.pct_change = true;

    publish(it.cb, &a, flags);
  }
}

void uoApp_publish_timerJson(const char *json) {
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

    publish(it.cb, json, flags);
  }
}
