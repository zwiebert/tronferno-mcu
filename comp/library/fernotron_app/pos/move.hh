/*
 * shutter_moving.h
 *
 *  Created on: 13.03.2020
 *      Author: bertw
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>
#include "fernotron/fer_pct.h"

#include "fernotron/pos/positions_dynamic.h"



extern struct shutter_timings st_def;

#define GRP_COUNT 7
#define MBR_COUNT 7
#define GRP_MAX 7
#define MBR_MAX 7
#define PCT_UP 100
#define PCT_DOWN 0

#define DEF_MV_UP_10 260
#define DEF_MV_DOWN_10 250
#define DEF_MV_SUN_DOWN_10 100

#include "move_buf.h"

// general control for moving
void fer_pos_checkStatus_whileMoving_periodic(int interval_ts);
void fer_pos_checkStatus_whileMoving();
uint16_t fer_simPos_calcMoveDuration_fromPctDiff_m(uint8_t g, uint8_t m, uint8_t curr_pct, uint8_t pct);


// start moving
/**
 * \brief Test if real movement happens configuration and current position (needs to be above sun-position)
 */
bool fer_pos_shouldMove_sunDown(uint8_t g, uint8_t m);
/**
 * \brief Test if real movement happens configuration and current position (needs to be at sun-position)
 */
bool fer_pos_shouldMove_sunUp(uint8_t g, uint8_t m);

// stop moving
void fer_pos_stop_mv(struct Fer_Move *Fer_Move, uint8_t g, uint8_t m, uint8_t pct);
void fer_pos_stop_mvi(struct Fer_Move *Fer_Move, uint8_t g, uint8_t m, uint32_t now_ts);
void fer_pos_stop_mm(Fer_GmSet *mm, uint32_t now_ts);
void fer_pos_stop_mvi_mm(struct Fer_Move *Fer_Move, Fer_GmSet *mm, uint32_t now_ts);

void fer_pos_POSITIONS_SAVE_cb(bool has_unsaved);


