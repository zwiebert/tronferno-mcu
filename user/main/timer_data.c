/*
 * timer_data.c
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */
#include "../user_config.h"
#include "timer_data.h"
#include "esp8266/data_flash2.h"

////////////////////////////////// public ////////////////////////////////////////////////////////////////////


bool ICACHE_FLASH_ATTR save_timer_data(DATA_TYPE *p, uint8_t g, uint8_t m) {
  bool result = false;

#if ENABLE_SPIFFS
  result = save_timer_data_spiffs(p, g, m);
#else
  result = save_timer_data_old(p, g, m);
#endif

  return result;
}

bool ICACHE_FLASH_ATTR read_timer_data(DATA_TYPE *p, uint8_t *g, uint8_t *m, bool wildcard) {
  bool result = false;
#if ENABLE_SPIFFS
  result = read_timer_data_spiffs(p, g, m, wildcard);
#else
  result = read_timer_data_old(p, g, m, wildcard);
#endif

  return result;
}
