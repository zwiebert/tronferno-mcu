#pragma once


#include <fernotron/fsb.h>
#include "stdbool.h"
#include "app_config/callbacks.h"

typedef enum  { CUAS_NONE, CUAS_SCANNING, CUAS_TIME_OUT, CUAS_SUCCESS } cuas_state_T;

bool cu_auto_set(uint16_t id, unsigned timeout_secs);
void cu_auto_set_check_timeout(void);
bool cu_auto_set_check(const fsbT *fsb);

cuas_state_T cuas_getState();

#ifndef cuas_ENABLE_cb
#define cuas_ENABLE_cb() // do nothing
#endif
#ifndef cuas_DISABLE_cb
#define cuas_DISABLE_cb() // do nothing
#endif


