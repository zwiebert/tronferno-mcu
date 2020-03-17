/*
 * types.h
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_TYPES_H_
#define COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_TYPES_H_

#include <stdint.h>
#include "stdbool.h"

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
int fer_gmByName_load(const char *name, const gm_bitmask_t *gm, int count);
int fer_gmByName_store(const char *name, gm_bitmask_t *gm, int count);

#endif /* COMPONENTS_FERNOTRON_INCLUDE_FERNOTRON_TYPES_H_ */
