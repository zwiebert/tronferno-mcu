/*
 * set_endpos.h
 *
 *  Created on: 28.11.2017
 *      Author: bertw
 */

#include "fernotron/fsb.h"

#ifndef USER_MAIN_SET_ENDPOS_H_
#define USER_MAIN_SET_ENDPOS_H_


bool sep_enable(fsbT *fsb);
void sep_disable(void);
bool sep_loop(void);
bool sep_is_enabled(void);

#endif /* USER_MAIN_SET_ENDPOS_H_ */
