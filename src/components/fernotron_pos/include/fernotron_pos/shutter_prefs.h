/*
 * shutter_preferences.h
 *
 *  Created on: 24.02.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_POS_INCLUDE_FERNOTRON_POS_SHUTTER_PREFS_H_
#define COMPONENTS_FERNOTRON_POS_INCLUDE_FERNOTRON_POS_SHUTTER_PREFS_H_

#include <stdint.h>
#include "stdbool.h"

enum shpMaskT { SHP_MVUT, SHP_MVDT, SHP_MVSPDT , SHP_len};

struct shutter_timings {
  uint16_t move_up_tsecs;
  uint16_t move_down_tsecs;
  uint16_t move_sundown_tsecs;
};


bool shuPref_save_timings(struct shutter_timings *src, uint8_t g, uint8_t m);
bool shuPref_read_timings(struct shutter_timings *dst, uint8_t g, uint8_t m);
bool shuPref_read_timings_wildcard(struct shutter_timings *dst, uint8_t *g, uint8_t *m);


#endif /* COMPONENTS_FERNOTRON_POS_INCLUDE_FERNOTRON_POS_SHUTTER_PREFS_H_ */
