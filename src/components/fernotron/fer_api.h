/*
 * fer_api.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#ifndef FERNOTRON_FER_API_H_
#define FERNOTRON_FER_API_H_

#include "fer_app_cfg.h"
#include "fernotron/fer_msg_plain.h"
#include "fernotron/timer_data.h"
#include "fernotron/fsb.h"
#include "time.h"

bool send_rtc_message(fsbT *fsb, time_t rtc);
bool send_timer_message(fsbT *fsb, time_t rtc, timer_data_t *td);
bool send_empty_timer_message(fsbT *fsb, time_t rtc);


bool timerString2bcd(const char *src, uint8_t *dst, uint16_t size_dst);
bool fill_timer_buf(fsbT *fsb, time_t rtc, timer_data_t *tdr);
bool fill_rtc_buf(fsbT *fsb, time_t rtc);



#endif /* FERNOTRON_FER_API_H_ */
