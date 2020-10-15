/*
 * timer_data_spiffs.h
 *
 *  Created on: 08.09.2018
 *      Author: bertw
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "fernotron_trx/timer_data.h"


bool save_timer_data_kvs(Fer_TimerData *p, uint8_t group, uint8_t member);
bool read_timer_data_kvs(Fer_TimerData *p, uint8_t *group, uint8_t *member, bool wildcard);
bool erase_timer_data_kvs(u8 g, u8 m);

