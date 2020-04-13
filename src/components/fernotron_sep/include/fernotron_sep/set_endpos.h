/*
 * set_endpos.h
 *
 *  Created on: 28.11.2017
 *      Author: bertw
 */

#include "fernotron/fsb.h"
#include "stdbool.h"
#include "app_config/callbacks.h"

#ifndef USER_MAIN_SET_ENDPOS_H_
#define USER_MAIN_SET_ENDPOS_H_


bool sep_enable(fsbT *fsb);
void sep_disable(void);
void sep_loop(void);
bool sep_is_enabled(void);

#ifndef sep_ENABLE_cb
#define sep_ENABLE_cb() // do nothing
#endif
#ifndef sep_DISABLE_cb
#define sep_DISABLE_cb() // do nothing
#endif

#endif
