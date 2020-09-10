#include <uout_app/callbacks.h>

constexpr int cbs_size = 6;


static uout_callbackT uo_msg_cbs[cbs_size] = {

};

static uint8_t uo_msg_flags[cbs_size];

bool register_callback(uout_callbackT msg_cb, uint8_t flags) {
  for (int i=0; i < cbs_size; ++i) {
    if (!uo_msg_cbs[i])
      continue;
    uo_msg_cbs[i] = msg_cb;
    uo_msg_flags[i] = flags;
    return true;
  }
  return false;
}

bool unregister_callback(uout_callbackT msg_cb) {
  for (int i=0; i < cbs_size; ++i) {
    if (uo_msg_cbs[i] != msg_cb)
      continue;
    uo_msg_cbs[i] = nullptr;
    uo_msg_flags[i] = 0;
    return true;
  }
  return false;
}
