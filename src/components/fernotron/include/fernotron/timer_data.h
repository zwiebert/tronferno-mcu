/*
 * timer_data.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "fernotron/fer_msg_attachment.h"

typedef struct {
  int16_t astro;      // minute offset of civil dusk, or 20000 for disables astro
  uint8_t bf;         // bitfield
  char daily[8 + 1];    // ASCII string of one daily timer
  char weekly[7 * 8 + 1]; // ASCII string of seven weekly timers
} timer_data_t;

#define td_initializer { 20000, 0, "", "" }
#define td_is_astro(td) ((td)->astro < 20000)
#define td_is_daily(td) ((td)->daily[0] != '\0')
#define td_is_weekly(td) ((td)->weekly[0] != '\0')

#define td_is_random(td)        (GET_BIT((td)->bf, flag_Random))
#define td_put_random(td, on)   (PUT_BIT((td)->bf, flag_Random, (on)))

#define td_is_sun_auto(td)      (GET_BIT((td)->bf, flag_SunAuto))
#define td_put_sun_auto(td, on) (PUT_BIT((td)->bf, flag_SunAuto, (on)))

