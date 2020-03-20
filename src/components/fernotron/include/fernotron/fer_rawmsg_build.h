/*
 * fer_prg.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_PRG_H_
#define USER_MAIN_FER_PRG_H_

#include <stdint.h>
#include "fsb.h"
#include "fer_msg_attachment.h"
#include <time.h>

void fmsg_raw_checksumsCreate(fer_rawMsg *m, fmsg_type t);
bool fmsg_raw_checksumsVerify(const fer_rawMsg *m, fmsg_type t);
void fmsg_raw_init(fer_rawMsg *msg);
void fmsg_raw_from_rtc(fer_rawMsg *msg, time_t rtc, bool rtc_only);
void fmsg_raw_from_flags(fer_rawMsg *msg, uint8_t flags, uint8_t mask);
void fmsg_raw_from_weeklyTimer(fer_rawMsg *msg, const uint8_t *wtimer_data);
void fmsg_raw_from_dailyTimer(fer_rawMsg *msg, const uint8_t *dtimer_data);
void fmsg_raw_from_astro(fer_rawMsg *msg, int mint_offset);
void fmsg_raw_footerCreate(fer_rawMsg *msg, const fsbT *fsb);

#endif /* USER_MAIN_FER_PRG_H_ */
