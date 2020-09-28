#pragma once

#include <stdint.h>
#include "fer_fsb.h"
#include "fer_msg_attachment.h"
#include <time.h>

void fer_msg_raw_checksumsCreate(fer_rawMsg *m, fer_msg_type t);
bool fer_msg_raw_checksumsVerify(const fer_rawMsg *m, fer_msg_type t);
void fer_msg_raw_init(fer_rawMsg *msg);
void fer_msg_raw_from_rtc(fer_rawMsg *msg, time_t rtc, bool rtc_only);
void fer_msg_raw_from_flags(fer_rawMsg *msg, uint8_t flags, uint8_t mask);
void fer_msg_raw_from_weeklyTimer(fer_rawMsg *msg, const uint8_t *wtimer_data);
void fer_msg_raw_from_dailyTimer(fer_rawMsg *msg, const uint8_t *dtimer_data);
void fer_msg_raw_from_astro(fer_rawMsg *msg, int mint_offset);
void fer_msg_raw_footerCreate(fer_rawMsg *msg, const fer_sbT *fsb);
