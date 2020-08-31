/*
 * stdbool.h
 *
 *  Created on: 14.03.2019
 *      Author: bertw
 */

#pragma once
#ifndef __cplusplus
#ifdef MCU_ESP8266
#include <c_types.h>
#endif

#ifndef true
typedef unsigned char bool;
#define BOOL            bool
#define true            (1)
#define false           (0)
#define TRUE            true
#define FALSE           false
#endif

#endif
