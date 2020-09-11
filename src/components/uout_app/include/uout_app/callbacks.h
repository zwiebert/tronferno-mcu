#pragma once
#include <stdint.h>
#include <uout_app/status_output.h>


struct uoApp_cbMsgT {
  enum { strT, void_ptrT, pchT, jsonT } msg_type;
  enum { pchBit, wsBit };
  union {
    void *void_ptr;
    const char *str;
    const char *json;
    const so_arg_pch_t *pch;
  };

  bool is_str() const { return strT == msg_type; }
  bool is_json() const { return jsonT == msg_type; }
  bool is_pch() const { return pchT == msg_type; }
};

typedef void (*uout_callbackT)(const uoApp_cbMsgT *msg);

bool uoApp_register_callback(uout_callbackT msg_cb, uint8_t flags = 0);
bool uoApp_unregister_callback(uout_callbackT msg_cb);


void uoApp_event_pinChange(const so_arg_pch_t *args);
