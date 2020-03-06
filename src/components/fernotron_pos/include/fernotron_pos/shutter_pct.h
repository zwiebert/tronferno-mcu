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

typedef uint8_t gm_bitmask_t[8];
#define gm_PutBit(gm, g, m, val) PUT_BIT(gm[(g)], (m), (val))
#define gm_ClrBit(gm, g, m) CLR_BIT(gm[(g)], (m))
#define gm_GetBit(gm, g, m) GET_BIT(gm[(g)], (m))
#define gm_SetBit(gm, g, m) SET_BIT(gm[(g)], (m))

typedef uint8_t shutterGroupPositionsT[8];

int ferPos_mGetPct(uint32_t a, uint8_t g, uint8_t m);
int ferPos_mGetMovingPct(uint32_t a, uint8_t g, uint8_t m);

int ferPos_printAllPcts(void);

int ferPos_mMove(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd);
int ferPos_mmMove(gm_bitmask_t mm, fer_cmd cmd);

int ferPos_mSetPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct);
int ferPos_mmSetPcts(gm_bitmask_t mm, uint8_t pct);

uint16_t ferPos_mCalcMoveDuration_fromPctDiff(uint8_t g, uint8_t m, uint8_t curr_pct, uint8_t pct);
uint8_t  ferPos_mCalcMovePct_fromDirectionAndDuration(uint8_t g, uint8_t m, bool direction_up, uint16_t time_s10);

void ferPos_loop(void);
void ferPos_init(void);


// load/store

// read/save one or more elements of type gm_bitmask_t
int ferPos_gmByName_load(const char *name, const gm_bitmask_t gm, int count);
int ferPos_gmByName_store(const char *name, gm_bitmask_t gm, int count);

int ferPos_pctsByGroup_load(uint8_t g, const shutterGroupPositionsT positions);
int ferPoas_pctsByGroup_store(uint8_t g, shutterGroupPositionsT positions);

#endif /* USER_MAIN_CURRENT_STATE_H_ */
