
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


