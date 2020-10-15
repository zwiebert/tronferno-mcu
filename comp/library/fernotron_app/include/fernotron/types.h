/**
 * \file    fernotron/types.h
 * \brief   XXX
 */
/*
 * types.h
 *
 *  Created on: 07.03.2020
 *      Author: bertw
 */

#pragma once

#ifdef __cplusplus
#include "fernotron/gm_set.hh"
#endif


#include <stdint.h>
#include "stdbool.h"
#include "utils_misc/int_macros.h"



// read/save one or more elements of type Fer_GmSet
bool fer_stor_gmSet_load(const char *name, gmSetT gm[], int count);
bool fer_stor_gmSet_save(const char *name, const gmSetT gm[], int count);


