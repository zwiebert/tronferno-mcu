#pragma once


#include <fernotron/fsb.h>
#include "stdbool.h"
#include "app/config/proj_app_cfg.h"


extern void (*fer_cuas_enable_disable_cb)(bool enable);
typedef enum  { FER_CUAS_NONE, FER_CUAS_SCANNING, FER_CUAS_TIME_OUT, FER_CUAS_SUCCESS } fer_cuas_state_T;

bool fer_cuas_set(const struct TargetDesc &td, uint16_t id, unsigned timeout_secs);
void fer_cuas_set_check_timeout(void);
bool fer_cuas_set_check(const fer_sbT *fsb);

fer_cuas_state_T fer_cuas_getState();



