/*
 * set_endpos.h
 *
 *  Created on: 28.11.2017
 *      Author: bertw
 */


#include "stdbool.h"
#include "app/config/proj_app_cfg.h"
#include <fernotron/api/fer_msg_send.hh>

#ifndef USER_MAIN_SET_ENDPOS_H_
#define USER_MAIN_SET_ENDPOS_H_

extern void (*fer_sep_enable_disable_cb)(bool enable);

bool fer_sep_enable(const struct TargetDesc &td, u32 a, u8 g, u8 m, fer_if_cmd cmd);
void fer_sep_disable(void);
void fer_sep_loop(void);
bool fer_sep_is_enabled(void);


#endif
