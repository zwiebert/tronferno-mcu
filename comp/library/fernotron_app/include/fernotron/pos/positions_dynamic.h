/*
 * positions_dynamic.h
 *
 *  Created on: 05.04.2020
 *      Author: bertw
 */

#pragma once

#include "fernotron/types.h"
#include "fernotron/fer_msg_plain.h"
#include "app/config/proj_app_cfg.h"


extern void (*fpos_POSITIONS_MOVE_cb)(bool has_moving);

// keep track of movement by simulation
int simPos_registerMovingShutter(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);
int simPos_registerMovingShutters(GmBitMask *mm, fer_cmd cmd);

int simPos_getPct_whileMoving(uint32_t a, uint8_t g, uint8_t m);
uint16_t simPos_calcMoveDuration_fromPctDiff_m(uint8_t g, uint8_t m, uint8_t curr_pct, uint8_t pct);
uint8_t  simPos_getPct_afterDuration(uint8_t g, uint8_t m, bool direction_up, uint16_t duration_ts);


void ferPos_loopCheckMoving();


void ferPos_loop(void);
void ferPos_init(void);

