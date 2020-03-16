/*
 * shutter_pct.h
 *
 *  Created on: 04.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_CURRENT_STATE_H_
#define USER_MAIN_CURRENT_STATE_H_

#include "main/common.h"
#include "fernotron/fer_rx_tx.h"
#include "fernotron/types.h"



typedef uint8_t shutterGroupPositionsT[8];

int ferPos_mGetPct(uint32_t a, uint8_t g, uint8_t m);
int ferPos_mGetMovingPct(uint32_t a, uint8_t g, uint8_t m);

int ferPos_printAllPcts(void);

int ferPos_move_m(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);
int ferPos_move_mm(gm_bitmask_t mm, fer_cmd cmd);

int ferPos_mSetPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);
int ferPos_mmSetPcts(gm_bitmask_t mm, uint8_t pct);

uint16_t ferPos_calcMoveDuration_fromPctDiff_m(uint8_t g, uint8_t m, uint8_t curr_pct, uint8_t pct);
uint8_t  ferPos_calcMovePct_fromDirectionAndDuration_m(uint8_t g, uint8_t m, bool direction_up, uint16_t time_s10);

void ferPos_loop(void);
void ferPos_init(void);


// load/store



int ferPos_pctsByGroup_load(uint8_t g, const shutterGroupPositionsT positions);
int ferPos_pctsByGroup_store(uint8_t g, shutterGroupPositionsT positions);

#endif /* USER_MAIN_CURRENT_STATE_H_ */
