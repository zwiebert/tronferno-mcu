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
#include "fernotron_trx/raw/fer_msg_plain.h"
#include "app_config/proj_app_cfg.h"
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
bool fer_pos_shouldStop_sunDown(u8 g, u8 m, u16 duration_ts);
void fer_pos_mvCheck_mvi(struct Fer_Move *Fer_Move);
int fer_pos_mvCheck_mv(struct Fer_Move *Fer_Move, unsigned now_ts);
void fer_pos_checkStatus_whileMoving_periodic(int interval_ts);
u16 fer_simPos_calcMoveDuration_fromPctDiff_m(u8 g, u8 m, u8 curr_pct, u8 pct);


// start moving
bool fer_pos_shouldMove_sunDown(uint8_t g, uint8_t m);
int fer_simPos_registerMovingShutter(u32 a, u8 g, u8 m, fer_if_cmd cmd);
int fer_simPos_registerMovingShutters(Fer_GmSet *mm, fer_if_cmd cmd);

// stop moving
void fer_pos_stop_mv(struct Fer_Move *Fer_Move, u8 g, u8 m, u8 pct);
void fer_pos_stop_mvi(struct Fer_Move *Fer_Move, u8 g, u8 m, u32 now_ts);
void fer_pos_stop_mm(Fer_GmSet *mm, u32 now_ts);
void fer_pos_stop_mvi_mm(struct Fer_Move *Fer_Move, Fer_GmSet *mm, u32 now_ts);




