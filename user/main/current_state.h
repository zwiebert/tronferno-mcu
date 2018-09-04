/*
 * current_state.h
 *
 *  Created on: 04.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_CURRENT_STATE_H_
#define USER_MAIN_CURRENT_STATE_H_

#include "common.h"
#include "fer_code.h"

int get_shutter_state(uint32_t a, fer_grp g, fer_memb m);
int set_shutter_state(uint32_t a, fer_grp g, fer_memb m, fer_cmd cmd);


#endif /* USER_MAIN_CURRENT_STATE_H_ */
