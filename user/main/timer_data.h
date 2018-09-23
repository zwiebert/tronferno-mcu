/*
 * timer_data.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_TIMER_DATA_H_
#define USER_MAIN_TIMER_DATA_H_

#include "fer_code.h"
#include "boolean.h"

typedef struct {
  int16_t astro;      // minute offset of civil dusk, or 20000 for disables astro
  uint8_t bf;         // bitfield
  char daily[8+1];    // ASCII string of one daily timer
  char weekly[7*8+1]; // ASCII string of seven weekly timers
} timer_data_t;

#define td_initializer { 20000, 0, "", "" }
#define td_is_astro(td) ((td)->astro < 20000)
#define td_is_daily(td) ((td)->daily[0] != '\0')
#define td_is_weekly(td) ((td)->weekly[0] != '\0')
#define td_is_random(td) ((td)->bf & (1 << flag_Random))
#define td_is_sun_auto(td) ((td)->bf & (1 << flag_SunAuto))


// save and read timer data on flash memory
// group  0...7
// member 0...7
// wildcard  if true, find the best matching data my return timer g=1 m=0 data on a search for g=1 m=1
//
// when saving data with group=0 or menber=0, all matching old data will be deleted. So g=0 m=0 will delete all other data, because it addresses all receivers
bool save_timer_data(timer_data_t *p, uint8_t group, uint8_t member);
bool read_timer_data(timer_data_t *p, uint8_t *group, uint8_t *member, bool wildcard);



#endif /* USER_MAIN_TIMER_DATA_H_ */
