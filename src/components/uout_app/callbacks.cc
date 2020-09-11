#include <uout_app/callbacks.h>
#include <misc/int_macros.h>

constexpr int cbs_size = 6;



struct {
  uout_callbackT cb;
  uint32_t flags;
} uo_msg_cbs[cbs_size];

static uint8_t uo_msg_flags[cbs_size];

bool uoApp_register_callback(uout_callbackT msg_cb, uint8_t flags) {
  for (auto it : uo_msg_cbs) {
    if (!it.cb)
      continue;
    it.cb = msg_cb;
    it.flags = flags;
    return true;
  }
  return false;
}

bool uoApp_unregister_callback(uout_callbackT msg_cb) {
  for (auto it : uo_msg_cbs) {
    if (it.cb != msg_cb)
      continue;
    it.cb = nullptr;
    it.flags = 0;
    return true;
  }
  return false;
}

void uoApp_event_pinChange(const so_arg_pch_t *args) {
  for (auto it : uo_msg_cbs) {
    if (it.cb && (it.flags & BIT(uoApp_cbMsgT::pchBit))) {
      uoApp_cbMsgT msg { .msg_type = uoApp_cbMsgT::pchT, .pch = args };
      it.cb(&msg);
    }

  }
  // TODO: Implement me
}
