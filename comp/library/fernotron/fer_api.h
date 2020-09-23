/*
 * fer_api.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#pragma once

#include "fer_app_cfg.h"
#include "fernotron/fer_msg_plain.h"
#include "fernotron/timer_data.h"
#include "fernotron/fsb.h"
#include "time.h"

bool fer_send_rtc_message(fer_sbT *fsb, time_t rtc);
bool fer_send_timer_message(fer_sbT *fsb, time_t rtc, Fer_TimerData *td);
bool fer_send_empty_timer_message(fer_sbT *fsb, time_t rtc);


bool timerString2bcd(const char *src, uint8_t *dst, uint16_t size_dst);
bool fill_timer_buf(fer_sbT *fsb, time_t rtc, Fer_TimerData *tdr);
bool fill_rtc_buf(fer_sbT *fsb, time_t rtc);


