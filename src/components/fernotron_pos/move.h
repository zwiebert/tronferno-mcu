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

struct mv {
  gm_bitmask_t mask;
  uint32_t start_time;
  bool direction_up : 1; //down=false, up=true
  bool sun_down : 1;
};

#define mv_SIZE 8
extern struct mv moving[mv_SIZE];
extern uint8_t moving_mask;
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


extern volatile u32 run_time_s10;
#define run_time_10(x) (run_time_s10 + 0)



int ferPos_getPct(u32 a, int g, int m);


// general control for moving
bool ferPos_mCalcSunStop(u8 g, u8 m, u16 time_s10);
void ferPos_mvCheck_mvi(u8 mvi);
void ferPos_mvCheck_iv(int interval_ts);
u16 ferPos_mCalcMoveDuration_fromPctDiff(u8 g, u8 m, u8 curr_pct, u8 pct);


// start moving
bool ferPos_mSunReadyToMoveDown(uint8_t g, uint8_t m);
int ferPos_mMove(u32 a, u8 g, u8 m, fer_cmd cmd);
int ferPos_mmMove(gm_bitmask_t mm, fer_cmd cmd);

// stop moving
void ferPos_markMovingAsComplete(struct mv *mv, u8 g, u8 m, u8 pct);
void stop_moving(int mvi, u8 g, u8 m, u32 rt);


#endif /* COMPONENTS_FERNOTRON_POS_SHUTTER_MOVEMENT_H_ */
