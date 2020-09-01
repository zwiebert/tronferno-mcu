/*
 * types.h
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#pragma once

#if 1
#include "fernotron/gm_bitmask.hh"
//using gm_bitmask_t = GmBitMask;
typedef GmBitMask gm_bitmask_t;

#include <stdint.h>
#include "stdbool.h"
#include "misc/int_macros.h"



#define gm_Initializer {}
#define gm_Clear(gm) (gm)->clear()
#define gm_GetByte(gm, g) (gm)->getByte(g)
#define gm_SetByte(gm, g, b) (gm)->setByte(g, b)
#define gm_PutBit(gm, g, m, val) (gm)->putBit(g, m, val)
#define gm_ClrBit(gm, g, m) (gm)->clearBit(g, m)
#define gm_GetBit(gm, g, m) (gm)->getBit(g, m)
#define gm_SetBit(gm, g, m) (gm)->setBit(g, m)
#define gm_isAllClear(gm) (gm)->isAllClear()
#define gm_fromNibbleCounters(gm, um) (gm)->fromNibbleCounters(um)

//bool gm_getNext(gm_bitmask_t *gm, uint8_t *gp, uint8_t *mp);

// read/save one or more elements of type gm_bitmask_t
bool fer_gmByName_load(const char *name, const gmBitMaskT *gm, int count);
bool fer_gmByName_store(const char *name, gmBitMaskT *gm, int count);


#else
#include <stdint.h>
#include "stdbool.h"
#include "misc/int_macros.h"

typedef uint8_t gm_bitmask_t[8];

#define gm_Initializer {0,}
#define gm_Clear(gm) (memset((gm), 0, sizeof(gm_bitmask_t)))
#define gm_GetByte(gm, g) ((*gm)[(g)]+0)
#define gm_SetByte(gm, g, b) (*gm)[(g)] = (b)
#define gm_PutBit(gm, g, m, val) PUT_BIT((*gm)[(g)], (m), (val))
#define gm_ClrBit(gm, g, m) CLR_BIT((*gm)[(g)], (m))
#define gm_GetBit(gm, g, m) GET_BIT((*gm)[(g)], (m))
#define gm_SetBit(gm, g, m) SET_BIT((*gm)[(g)], (m))

bool gm_getNext(gm_bitmask_t *gm, uint8_t *gp, uint8_t *mp);

bool gm_isAllClear(gm_bitmask_t *gm);
int gm_countSetBits(gm_bitmask_t *gm);

// read/save one or more elements of type gm_bitmask_t
bool fer_gmByName_load(const char *name, const gm_bitmask_t *gm, int count);
bool fer_gmByName_store(const char *name, gm_bitmask_t *gm, int count);

void gm_fromNibbleCounters(gm_bitmask_t *gm, uint32_t um);

#endif
