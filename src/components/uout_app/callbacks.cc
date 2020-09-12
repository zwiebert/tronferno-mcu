
#include <misc/int_macros.h>

#define UOUT_PROTECTED
#include <uout/callbacks.h>
#include <uout_app/callbacks.h>

void uoApp_publish_pinChange(const so_arg_pch_t args) {
  for (auto it : uoCb_cbs) {
    if (!it.cb)
      continue;
    if (!it.flags.evt.pin_change)
      continue;
    if (!it.flags.fmt.obj)
      continue;

    uoCb_msgT  msg { .cv_ptr = &args, .flags = { } };
    msg.flags.fmt.obj = true;
    msg.flags.evt.pin_change = true;

    it.cb(msg);
  }
}

static void publish(uoCb_cbT cb, const void *ptr, uo_flagsT flags) {
  uoCb_msgT  msg { .cv_ptr = ptr, .flags = flags };
  cb(msg);
}

void uoApp_publish_gmpJson(const char *json) {
  for (auto it : uoCb_cbs) {
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

  for (auto it : uoCb_cbs) {
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

