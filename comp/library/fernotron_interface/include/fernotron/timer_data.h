/*
 * timer_data.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  int16_t astro;      // minute offset of civil dusk, or 20000 for disables astro
  union {
    struct {
      uint8_t random :1;
      uint8_t unused_1 :1;
      uint8_t dst :1;
      uint8_t unused_3 :1;
      uint8_t unused_4 :1;
      uint8_t unused_5 :1;
      uint8_t unused_6 :1;
      uint8_t sunAuto :1;
    }flags;
  uint8_t bf;         // bitfield
  };
  char daily[8 + 1];    // ASCII string of one daily timer
  char weekly[7 * 8 + 1]; // ASCII string of seven weekly timers
} Fer_TimerData;

#define fer_td_initializer { 20000, {0}, "", "" }
#define fer_td_is_astro(td) ((td)->astro < 20000)
#define fer_td_is_daily(td) ((td)->daily[0] != '\0')
#define fer_td_is_weekly(td) ((td)->weekly[0] != '\0')

#define fer_td_is_random(td)        (!!(td)->flags.random)
#define fer_td_put_random(td, on)   ((td)->flags.random = (on))

#define fer_td_is_sun_auto(td)      (!!(td)->flags.sunAuto)
#define fer_td_put_sun_auto(td, on) ((td)->flags.sunAuto = (on))

