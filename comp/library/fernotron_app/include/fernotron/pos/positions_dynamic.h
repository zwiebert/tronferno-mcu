/*
 * positions_dynamic.h
 *
 *  Created on: 05.04.2020
 *      Author: bertw
 */

#pragma once

#include "fernotron/types.h"
#include <fernotron/trx/fer_trx_c_api.h>
#include "app/config/proj_app_cfg.h"


extern void (*fer_pos_POSITIONS_MOVE_cb)(bool has_moving);

// keep track of movement by simulation
int fer_simPos_registerMovingShutter(uint32_t a, uint8_t g, uint8_t m, fer_if_cmd cmd);
int fer_simPos_registerMovingShutters(Fer_GmSet *mm, fer_if_cmd cmd);

int fer_simPos_getPct_whileMoving(uint32_t a, uint8_t g, uint8_t m);
uint16_t fer_simPos_calcMoveDuration_fromPctDiff_m(uint8_t g, uint8_t m, uint8_t curr_pct, uint8_t pct);
uint8_t  fer_simPos_getPct_afterDuration(uint8_t g, uint8_t m, bool direction_up, uint16_t duration_ts);


void fer_pos_loopCheckMoving();


void fer_pos_loop(void);
void fer_pos_init(void);

