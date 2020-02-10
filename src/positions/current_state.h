/*
 * current_state.h
 *
 *  Created on: 04.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_CURRENT_STATE_H_
#define USER_MAIN_CURRENT_STATE_H_

#include "main/common.h"
#include "fernotron/fer_code.h"
#include "positions/shutter_state.h"

int get_shutter_state(u32 a, u8 g, u8 m);
int modify_shutter_positions(gm_bitmask_t mm, u8 p);
int print_shutter_positions(void);

int currentState_Move(u32 a, u8 g, u8 m, fer_cmd cmd);

void currentState_loop(void);
void currentState_init(void);


#endif /* USER_MAIN_CURRENT_STATE_H_ */
