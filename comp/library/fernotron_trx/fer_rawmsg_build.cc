/*
 * fer_prg.c
 *
 * Created: 14.08.2017 17:32:28
 *  Author: bertw
 */
#include <fernotron_trx/raw/fer_rawmsg_build.h>
#include <fernotron_trx/raw/fer_msg_plain.h>
#include "fernotron_trx/astro.h"
#include <string.h>
#include "fer_app_cfg.h"
#include "fer_api.h"
#include "debug/dbg.h"

#include "fernotron_trx/raw/fer_rawmsg_buffer.h"
//#include "app_misc/rtc.h"
//#include "fernotron/auto/astro.h"
//#include "app_settings/config.h"
#include "utils_misc/bcd.h"

/* verify all checksums of message in place. */
bool  fer_msg_raw_checksumsVerify(const struct fer_raw_msg *m, fer_msg_type t) {
	int line, column;
	u8 checksum = 0;


	for (column=0; column < FER_CMD_BYTE_CT; ++column) {

		if (column == FER_CMD_BYTE_CT - 1)
			 if (m->cmd.bd[column] != checksum)
				 return false;

		checksum += m->cmd.bd[column];
	}

	if (MSG_TYPE_PLAIN == t)
		return true;

	for (column = 0; column < FER_PRG_BYTE_CT; ++ column) {

		if (column == FER_PRG_BYTE_CT - 1)
			 if (m->rtc.bd[column] != checksum)
				 return false;

		checksum += m->rtc.bd[column];
	}

	if (MSG_TYPE_RTC == t)
		return true;

	for (line = 0; line < FER_FPR_TIMER_HEIGHT; ++line) {
		for (column = 0; column < FER_PRG_BYTE_CT; ++column) {

			if (column == FER_PRG_BYTE_CT - 1)
				 if (m->wdtimer.bd[line][column] != checksum)
					 return false;

			checksum += m->wdtimer.bd[line][column];
		}
	}

	for (line = 0; line < FER_FPR_ASTRO_HEIGHT; ++line) {
		for (column = 0; column < FER_PRG_BYTE_CT; ++column) {

			if (column == FER_PRG_BYTE_CT - 1)
				 if (m->astro[line][column] != checksum)
					 return false;

			checksum += m->astro[line][column];
		}
	}
	return true;
}


/* create all checksums of message in place. */
void  fer_msg_raw_checksumsCreate(struct fer_raw_msg *m, fer_msg_type t) {
	int line, column;
	u8 checksum = 0;

	for (column=0; column < FER_CMD_BYTE_CT; ++column) {

		if (column == FER_CMD_BYTE_CT - 1)
			 m->cmd.sd.checkSum = checksum;

		checksum += m->cmd.bd[column];
	}

	if (MSG_TYPE_PLAIN == t)
		return;

	for (column = 0; column < FER_PRG_BYTE_CT; ++ column) {

		if (column == FER_PRG_BYTE_CT - 1)
			 m->rtc.sd.checkSum = checksum;

		checksum += m->rtc.bd[column];
	}

	if (MSG_TYPE_RTC == t)
		return;

	for (line = 0; line < FER_FPR_TIMER_HEIGHT; ++line) {
		for (column = 0; column < FER_PRG_BYTE_CT; ++column) {

			if (column == FER_PRG_BYTE_CT - 1)
				 m->wdtimer.bd[line][column] = checksum;

			checksum += m->wdtimer.bd[line][column];
		}
	}

	for (line = 0; line < FER_FPR_ASTRO_HEIGHT; ++line) {
		for (column = 0; column < FER_PRG_BYTE_CT; ++column) {

			if (column == FER_PRG_BYTE_CT - 1)
				 m->astro[line][column] = checksum;

			checksum += m->astro[line][column];
		}
	}

}

static void  disable_timer(u8 d[][FER_PRG_BYTE_CT], u8 lines) {
  int line, col;
  for (line = 0; line < lines; ++line) {
    for (col = 0; col < FER_FPR_TIMER_WIDTH; ++col) {
      d[line][col] = 0xff;
      ++col;
      d[line][col] = 0x0f;
    }
  }
}

static void  write_wtimer(u8 d[][FER_PRG_BYTE_CT], const u8 *wtimer_data) {
  #ifdef WEEK_STARTS_AT_SUNDAY
  memcpy(d[0], wtimer_data, FER_FPR_TIMER_WIDTH);
  memcpy(d[1],  (wtimer_data += FER_FPR_TIMER_WIDTH), FER_FPR_TIMER_WIDTH);
  memcpy(d[2],  (wtimer_data += FER_FPR_TIMER_WIDTH), FER_FPR_TIMER_WIDTH);
  memcpy(d[3],  (wtimer_data += FER_FPR_TIMER_WIDTH), FER_FPR_TIMER_WIDTH - FER_FPR_TIMER_STAMP_WIDTH);
  #else
  //reorder. input week starts with monday. output with sunday
  memcpy(d[0] + FER_FPR_TIMER_STAMP_WIDTH, wtimer_data, FER_FPR_TIMER_STAMP_WIDTH);
  memcpy(d[1],  (wtimer_data += FER_FPR_TIMER_STAMP_WIDTH), FER_FPR_TIMER_WIDTH);
  memcpy(d[2],  (wtimer_data += FER_FPR_TIMER_WIDTH), FER_FPR_TIMER_WIDTH);
  memcpy(d[3],  (wtimer_data += FER_FPR_TIMER_WIDTH), FER_FPR_TIMER_STAMP_WIDTH);
  memcpy(d[0], (wtimer_data += FER_FPR_TIMER_STAMP_WIDTH), FER_FPR_TIMER_STAMP_WIDTH);
  #endif
}

static void  write_dtimer(u8 d[FER_FPR_TIMER_STAMP_WIDTH], const u8 *dtimer_data) {
    memcpy(d, dtimer_data, FER_FPR_TIMER_STAMP_WIDTH);
}

static inline fer_rtc_wday getFerRtcWday(const struct tm *t) {
  return static_cast<fer_rtc_wday>(t->tm_wday == 0 ? 7 : t->tm_wday);
}

void fer_msg_rtc_from_time(fer_rtc_sd *msgPtr, time_t now, bool rtc_only) {
  struct tm *t = localtime(&now);

  auto &msg = *msgPtr;
  msg.secs = dec2bcd(t->tm_sec);
  msg.mint = dec2bcd(t->tm_min);
  msg.hour = dec2bcd(t->tm_hour);
  msg.mday = dec2bcd(t->tm_mday);
  msg.mont = dec2bcd(t->tm_mon + 1);
  msg.wd2.sd.wday = getFerRtcWday(t);
  msg.wd2.sd.rtc_only = rtc_only;

  msg.wDayMask = BIT(t->tm_wday);
  msg.flags.bits.dst = t->tm_isdst;
}

static void  write_flags(u8 *dst, u8 flags, u8 mask) {
	*dst = (*dst & ~mask) | (flags & mask);
}

static void  write_lastline(u32 a, u8 d[FER_PRG_BYTE_CT]) {
  d[0] = 0x00;
  d[1] = GET_BYTE_2(a);
  d[2] = GET_BYTE_1(a);
  d[3] = GET_BYTE_0(a);
  d[4] = 0x53;
  d[5] = 0x00;
  d[6] = 0x10;
  d[7] = 0x11;
  d[8] = 0x05;
}

void fer_msg_raw_init(struct fer_raw_msg *m, fer_msg_type t) {
  memset(&m->cmd, 0, sizeof(m->cmd));
  if (t == MSG_TYPE_RTC || t == MSG_TYPE_TIMER)
    memset(&m->rtc, 0, sizeof(m->rtc));
  if (t == MSG_TYPE_TIMER)
    disable_timer(m->wdtimer.bd, FER_FPR_TIMER_HEIGHT + FER_FPR_ASTRO_HEIGHT);
}
void  fer_msg_raw_from_flags(fer_rawMsg *msg, u8 flags, u8 mask) {
  write_flags(&msg->rtc.sd.flags.bd, flags, mask);
}
void  fer_msg_raw_from_weeklyTimer(fer_rawMsg *msg, const u8 *wtimer_data) {
  write_wtimer(msg->wdtimer.bd, wtimer_data);
}
void  fer_msg_raw_from_dailyTimer(fer_rawMsg *msg, const u8 *dtimer_data) {
  write_dtimer(&msg->wdtimer.bd[3][FER_FPR_DAILY_START_COL], dtimer_data);
}
void  fer_msg_raw_from_astro(fer_rawMsg *msg, int mint_offset) {
  fer_astro_write_data(msg->astro, mint_offset);
}
void  fer_msg_raw_footerCreate(fer_rawMsg *msg, u32 a) {
	write_lastline(a, msg->last);
}    
