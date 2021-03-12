/**
 * \file   fernotron/pos/shutter_prefs.h
 * \brief  store persistent data for a shutter or group
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

bool fer_shPref_strByM_store(const char *s, const char *tag, uint8_t g, uint8_t m);
bool fer_shPref_strByM_load(char *d, unsigned size, const char *tag, uint8_t g, uint8_t m);
typedef void (*fer_shPref_strCallBackT)(const struct TargetDesc &td, const char *tag, const char *val);
int fer_shPref_strByM_forEach(const struct TargetDesc &td, const char *tag, uint8_t g, uint8_t m, fer_shPref_strCallBackT callback);
bool fer_shPref_strByM_forOne(const struct TargetDesc &td, const char *tag, uint8_t g, uint8_t m, fer_shPref_strCallBackT callback);

bool fer_pos_prefByM_store(struct shutter_timings *src, uint8_t g, uint8_t m);
bool fer_pos_prefByM_load(struct shutter_timings *dst, uint8_t g, uint8_t m);
bool fer_pos_prefByWildcard_load(struct shutter_timings *dst, uint8_t *g, uint8_t *m);

