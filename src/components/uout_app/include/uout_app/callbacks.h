#pragma once
#include <stdint.h>


typedef void (*uout_callbackT)(const char *msg, uint8_t flags);

bool register_callback(uout_callbackT msg_cb, int flags);
bool unregister_callback(uout_callbackT msg_cb);
