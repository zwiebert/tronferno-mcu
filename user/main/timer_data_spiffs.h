/*
 * timer_data_spiffs.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#ifndef USER_MAIN_TIMER_DATA_SPIFFS_H_
#define USER_MAIN_TIMER_DATA_SPIFFS_H_

#include "esp8266/data_flash2.h"

bool save_timer_data_spiffs(DATA_TYPE *p, uint8_t group, uint8_t member);
bool read_timer_data_spiffs(DATA_TYPE *p, uint8_t *group, uint8_t *member, bool wildcard);

#endif /* USER_MAIN_TIMER_DATA_SPIFFS_H_ */
