#pragma once


#include <fernotron/fsb.h>
#include "stdbool.h"
#include "app/config/proj_app_cfg.h"

extern void (*cuas_enable_disable_cb)(bool enable);

typedef enum  { CUAS_NONE, CUAS_SCANNING, CUAS_TIME_OUT, CUAS_SUCCESS } cuas_state_T;

bool cu_auto_set(uint16_t id, unsigned timeout_secs);
void cu_auto_set_check_timeout(void);
bool cu_auto_set_check(const fsbT *fsb);

cuas_state_T cuas_getState();



