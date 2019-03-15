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

int get_shutter_state(uint32_t a, uint8_t g, uint8_t m);
int set_shutter_state(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);
int modify_shutter_positions(gm_bitmask_t mm, uint8_t p);
int print_shutter_positions(void);


#endif /* USER_MAIN_CURRENT_STATE_H_ */
