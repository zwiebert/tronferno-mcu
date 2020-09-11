
#include <misc/int_macros.h>

#define UOUT_PROTECTED
#include <uout/callbacks.h>
#include <uout_app/callbacks.h>

void uoApp_event_pinChange(const so_arg_pch_t *args) {
  for (auto it : uoCb_cbs) {
    if (it.cb && (it.flags & BIT(UOACB_MFB(pch)))) {
      uoCb_msgT msg { .cv_ptr = args, .msg_type = UOACB_MT(pch) };
      it.cb(&msg);
    }
  }
}
