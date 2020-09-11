#pragma once
#include <stdint.h>


typedef void (*uout_callbackT)(const char *msg, void *args);

bool register_callback(uout_callbackT msg_cb, uint8_t flags = 0);
bool unregister_callback(uout_callbackT msg_cb);
