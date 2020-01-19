/*
 * timer_data.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_TIMER_DATA_H_
#define USER_MAIN_TIMER_DATA_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  i16 astro;      // minute offset of civil dusk, or 20000 for disables astro
  u8 bf;         // bitfield
  char daily[8+1];    // ASCII string of one daily timer
  char weekly[7*8+1]; // ASCII string of seven weekly timers
} timer_data_t;


#include "fernotron/fer_code.h"



#define td_initializer { 20000, 0, "", "" }
#define td_is_astro(td) ((td)->astro < 20000)
#define td_is_daily(td) ((td)->daily[0] != '\0')
#define td_is_weekly(td) ((td)->weekly[0] != '\0')

#define td_is_random(td)        (GET_BIT((td)->bf, flag_Random))
#define td_put_random(td, on)   (PUT_BIT((td)->bf, flag_Random, (on)))

#define td_is_sun_auto(td)      (GET_BIT((td)->bf, flag_SunAuto))
#define td_put_sun_auto(td, on) (PUT_BIT((td)->bf, flag_SunAuto, (on)))

// save and read timer data on flash memory
// group  0...7
// member 0...7
// wildcard  if true, find the best matching data my return timer g=1 m=0 data on a search for g=1 m=1
//
// when saving data with group=0 or menber=0, all matching old data will be deleted. So g=0 m=0 will delete all other data, because it addresses all receivers
bool save_timer_data(timer_data_t *p, u8 group, u8 member);
bool read_timer_data(timer_data_t *p, u8 *group, u8 *member, bool wildcard);

extern bool timer_data_changed; // set by sabe_timer_data - cleared by code reading it



#endif /* USER_MAIN_TIMER_DATA_H_ */
