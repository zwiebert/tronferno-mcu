/*
 * types.h
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#pragma once

#ifdef __cplusplus
#include "fernotron/gm_bitmask.hh"
#endif


#include <stdint.h>
#include "stdbool.h"
#include "misc/int_macros.h"



// read/save one or more elements of type GmBitMask
bool fer_gmByName_load(const char *name, const gmBitMaskT *gm, int count);
bool fer_gmByName_store(const char *name, gmBitMaskT *gm, int count);


