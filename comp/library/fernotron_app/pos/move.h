/*
 * shutter_moving.h
 *
 *  Created on: 13.03.2020
 *      Author: bertw
 */

#pragma once

#include "stdbool.h"
#include <stdint.h>
#include "fernotron/types.h"
#include "fernotron/fer_msg_plain.h"
#include "app/config/proj_app_cfg.h"
#include "fernotron/pos/positions_dynamic.h"

#include "move_buf.h"

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

// general control for moving
bool ferPos_shouldStop_sunDown(u8 g, u8 m, u16 duration_ts);
void ferPos_mvCheck_mvi(struct mv *mv);
int ferPos_mvCheck_mv(struct mv *mv, unsigned now_ts);
void ferPos_checkStatus_whileMoving_periodic(int interval_ts);
u16 simPos_calcMoveDuration_fromPctDiff_m(u8 g, u8 m, u8 curr_pct, u8 pct);


// start moving
bool ferPos_shouldMove_sunDown(uint8_t g, uint8_t m);
int simPos_registerMovingShutter(u32 a, u8 g, u8 m, fer_cmd cmd);
int simPos_registerMovingShutters(GmBitMask *mm, fer_cmd cmd);

// stop moving
void ferPos_stop_mv(struct mv *mv, u8 g, u8 m, u8 pct);
void ferPos_stop_mvi(struct mv *mv, u8 g, u8 m, u32 now_ts);
void ferPos_stop_mm(GmBitMask *mm, u32 now_ts);
void ferPos_stop_mvi_mm(struct mv *mv, GmBitMask *mm, u32 now_ts);




