/*
 * shutter_moving.h
 *
 *  Created on: 13.03.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_POS_SHUTTER_MOVEMENT_H_
#define COMPONENTS_FERNOTRON_POS_SHUTTER_MOVEMENT_H_

#include "stdbool.h"
#include <stdint.h>
#include "fernotron/types.h"

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
u16 ferPos_calcMoveDuration_fromPctDiff_m(u8 g, u8 m, u8 curr_pct, u8 pct);


// start moving
bool ferPos_shouldMove_sunDown(uint8_t g, uint8_t m);
int ferPos_registerMovingShutter(u32 a, u8 g, u8 m, fer_cmd cmd);
int ferPos_registerMovingShutters(gm_bitmask_t *mm, fer_cmd cmd);

// stop moving
void ferPos_stop_mv(struct mv *mv, u8 g, u8 m, u8 pct);
void ferPos_stop_mvi(struct mv *mv, u8 g, u8 m, u32 now_ts);
void ferPos_stop_mm(gm_bitmask_t *mm, u32 now_ts);
void ferPos_stop_mvi_mm(struct mv *mv, gm_bitmask_t *mm, u32 now_ts);

#endif /* COMPONENTS_FERNOTRON_POS_SHUTTER_MOVEMENT_H_ */
