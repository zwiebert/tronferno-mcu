/*
 * shutter_preferences.h
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include "stdbool.h"

enum shpMaskT { SHP_MVUT, SHP_MVDT, SHP_MVSPDT , SHP_len};

struct shutter_timings {
  uint16_t move_up_tsecs;
  uint16_t move_down_tsecs;
  uint16_t move_sundown_tsecs;
};

bool ferSp_strByM_store(const char *s, const char *tag, uint8_t g, uint8_t m);
bool ferSp_strByM_load(char *d, unsigned size, const char *tag, uint8_t g, uint8_t m);
typedef void (*ferSp_strCallBackT)(const char *tag, const char *val);
int ferSp_strByM_forEach(const char *tag, uint8_t g, uint8_t m, ferSp_strCallBackT callback);
bool ferSp_strByM_forOne(const char *tag, uint8_t g, uint8_t m, ferSp_strCallBackT callback);

bool ferPos_prefByM_store(struct shutter_timings *src, uint8_t g, uint8_t m);
bool ferPos_prefByM_load(struct shutter_timings *dst, uint8_t g, uint8_t m);
bool ferPos_prefByWildcard_load(struct shutter_timings *dst, uint8_t *g, uint8_t *m);

