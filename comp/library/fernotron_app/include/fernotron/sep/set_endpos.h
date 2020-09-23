/*
 * set_endpos.h
 *
 *  Created on: 28.11.2017
 *      Author: bertw
 */

#include "fernotron/fsb.h"
#include "stdbool.h"
#include "app/config/proj_app_cfg.h"

#ifndef USER_MAIN_SET_ENDPOS_H_
#define USER_MAIN_SET_ENDPOS_H_

extern void (*fer_sep_enable_disable_cb)(bool enable);

bool fer_sep_enable(const struct TargetDesc &td, fer_sbT *fsb);
void fer_sep_disable(void);
void fer_sep_loop(void);
bool fer_sep_is_enabled(void);


#endif
