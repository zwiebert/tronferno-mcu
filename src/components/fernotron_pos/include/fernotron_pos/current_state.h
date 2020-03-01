/*
 * current_state.h
 *
 *  Created on: 04.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_CURRENT_STATE_H_
#define USER_MAIN_CURRENT_STATE_H_

#include "main/common.h"
#include "fernotron/fer_rx_tx.h"

#include "shutter_state.h"

int currentState_getShutterPct(uint32_t a, uint8_t g, uint8_t m);
int currentState_getMovingPct(uint32_t a, uint8_t g, uint8_t m);
int currentState_modifyShutterPositions(gm_bitmask_t mm, uint8_t pct);
int currentState_printShutterPositions(void);

int currentState_Move(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);
int currentState_mmMove(gm_bitmask_t mm, fer_cmd cmd);

int currentState_setShutterPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);
uint16_t currentState_mvCalcTime10(uint8_t g, uint8_t m, uint8_t curr_pct, uint8_t pct);
uint8_t  currentState_mvCalcPct(uint8_t g, uint8_t m, bool direction_up, uint16_t time_s10);

void currentState_loop(void);
void currentState_init(void);


#endif /* USER_MAIN_CURRENT_STATE_H_ */
