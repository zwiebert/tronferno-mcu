/*
 * fer_prg.c
 *
 * Created: 14.08.2017 17:32:28
 *  Author: bertw
 */

#include <string.h>

#include "common.h"
#include "fer.h"
#include "counter.h"
#include "utils.h"
#include "fer_code.h"
#include "rtc.h"
#include "astro.h"


/* verify all checksums of message in place. */
bool ICACHE_FLASH_ATTR fer_msg_verify_checksums(const struct fer_msg *m, fer_msg_type t) {
	int line, column;
	uint8_t checksum = 0;


	for (column=0; column < bytesPerCmdPacket; ++column) {

		if (column == bytesPerCmdPacket - 1)
			 if (m->cmd[column] != checksum)
				 return false;

		checksum += m->cmd[column];
	}

	if (MSG_TYPE_PLAIN == t)
		return true;

	for (column = 0; column < bytesPerPrgLine; ++ column) {

		if (column == bytesPerPrgLine - 1)
			 if (m->rtc[column] != checksum)
				 return false;

		checksum += m->rtc[column];
	}

	if (MSG_TYPE_RTC == t)
		return true;

	for (line = 0; line < FPR_TIMER_HEIGHT; ++line) {
		for (column = 0; column < bytesPerPrgLine; ++column) {

			if (column == bytesPerPrgLine - 1)
				 if (m->wdtimer[line][column] != checksum)
					 return false;

			checksum += m->wdtimer[line][column];
		}
	}

	for (line = 0; line < FPR_ASTRO_HEIGHT; ++line) {
		for (column = 0; column < bytesPerPrgLine; ++column) {

			if (column == bytesPerPrgLine - 1)
				 if (m->astro[line][column] != checksum)
					 return false;

			checksum += m->astro[line][column];
		}
	}
	return true;
}


/* create all checksums of message in place. */
void ICACHE_FLASH_ATTR fer_msg_create_checksums(struct fer_msg *m, fer_msg_type t) {
	int line, column;
	uint8_t checksum = 0;

	for (column=0; column < bytesPerCmdPacket; ++column) {

		if (column == bytesPerCmdPacket - 1)
			 m->cmd[column] = checksum;

		checksum += m->cmd[column];
	}

	if (MSG_TYPE_PLAIN == t)
		return;

	for (column = 0; column < bytesPerPrgLine; ++ column) {

		if (column == bytesPerPrgLine - 1)
			 m->rtc[column] = checksum;

		checksum += m->rtc[column];
	}

	if (MSG_TYPE_RTC == t)
		return;

	for (line = 0; line < FPR_TIMER_HEIGHT; ++line) {
		for (column = 0; column < bytesPerPrgLine; ++column) {

			if (column == bytesPerPrgLine - 1)
				 m->wdtimer[line][column] = checksum;

			checksum += m->wdtimer[line][column];
		}
	}

	for (line = 0; line < FPR_ASTRO_HEIGHT; ++line) {
		for (column = 0; column < bytesPerPrgLine; ++column) {

			if (column == bytesPerPrgLine - 1)
				 m->astro[line][column] = checksum;

			checksum += m->astro[line][column];
		}
	}

}

void ICACHE_FLASH_ATTR disable_timer(uint8_t d[][FER_PRG_BYTE_CT], uint8_t lines) {
	int line, col;
	for (line = 0; line < lines; ++line) {
		for (col = 0; col < FPR_TIMER_WIDTH; ++col) {
			d[line][col] = 0xff;
			++col;
			d[line][col] = 0x0f;
		}
	}
}


void ICACHE_FLASH_ATTR init_prgData(struct fer_msg *m) {
	bzero(m->rtc, FPR_RTC_WIDTH);
	disable_timer(m->wdtimer, FPR_TIMER_HEIGHT + FPR_ASTRO_HEIGHT);
}



void ICACHE_FLASH_ATTR write_wtimer(uint8_t d[][FER_PRG_BYTE_CT], const uint8_t *wtimer_data) {
  #ifdef WEEK_STARTS_AT_SUNDAY
  memcpy(d[0], wtimer_data, FPR_TIMER_WIDTH);
  memcpy(d[1],  (wtimer_data += FPR_TIMER_WIDTH), FPR_TIMER_WIDTH);
  memcpy(d[2],  (wtimer_data += FPR_TIMER_WIDTH), FPR_TIMER_WIDTH);
  memcpy(d[3],  (wtimer_data += FPR_TIMER_WIDTH), FPR_TIMER_WIDTH - FPR_TIMER_STAMP_WIDTH);
  #else
  //reorder. input week starts with monday. output with sunday
  memcpy(d[0] + FPR_TIMER_STAMP_WIDTH, wtimer_data, FPR_TIMER_STAMP_WIDTH);
  memcpy(d[1],  (wtimer_data += FPR_TIMER_STAMP_WIDTH), FPR_TIMER_WIDTH);
  memcpy(d[2],  (wtimer_data += FPR_TIMER_WIDTH), FPR_TIMER_WIDTH);
  memcpy(d[3],  (wtimer_data += FPR_TIMER_WIDTH), FPR_TIMER_STAMP_WIDTH);
  memcpy(d[0], (wtimer_data += FPR_TIMER_STAMP_WIDTH), FPR_TIMER_STAMP_WIDTH);
  #endif
}

void ICACHE_FLASH_ATTR write_dtimer(uint8_t d[FPR_TIMER_STAMP_WIDTH], const uint8_t *dtimer_data) {
    memcpy(d, dtimer_data, FPR_TIMER_STAMP_WIDTH);
}

void ICACHE_FLASH_ATTR txbuf_write_dtimer(const uint8_t *dtimer_data) {
  write_dtimer(get_sendPrgBufLine(FPR_DAILY_START_ROW) + FPR_DAILY_START_COL, dtimer_data);
}

void ICACHE_FLASH_ATTR txbuf_write_wtimer(const uint8_t *wtimer_data) {
  write_wtimer(txmsg->wdtimer, wtimer_data);
}


void ICACHE_FLASH_ATTR txbuf_write_astro(int mint_offset) {
  write_astro(txmsg->astro, mint_offset);
}


void ICACHE_FLASH_ATTR write_rtc(uint8_t d[FPR_RTC_WIDTH], bool rtc_only) {
	time_t timer = time(NULL);
	struct tm *t = localtime(&timer);


	d[fpr0_RTC_secs] = dec2bcd(t->tm_sec);
	d[fpr0_RTC_mint] = dec2bcd(t->tm_min);
	d[fpr0_RTC_hour] = dec2bcd(t->tm_hour);
	d[fpr0_RTC_days] = dec2bcd(t->tm_mday);
	d[fpr0_RTC_mont] = dec2bcd(t->tm_mon + 1);
	d[fpr0_RTC_wday] = (rtc_only ? 0x80 : 0x00) | (t->tm_wday == 0 ? 7 : t->tm_wday); // monday==1 ... sunday==7
	d[fpr0_RTC_wdayMask] = (1 << t->tm_wday);
	PUT_BIT(d[fpr0_FlagBits], flag_DST, t->tm_isdst);
}

void ICACHE_FLASH_ATTR write_flags(uint8_t d[FPR_RTC_WIDTH], uint8_t flags, uint8_t mask) {
#if 1
	int i;

	for (i = 0; i < 8; ++i) {
		if (mask & 1) {
			PUT_BIT(d[fpr0_FlagBits], i, (flags & 1));
		}

		mask >>= 1;
		flags >>= 1;
	}

#else // FIXME: did I forgot how to use bit masks
	d[fpr0_FlagBits] |= (flags & mask);
	d[fpr0_FlagBits] ^= ~(flags & mask);
#endif

}

void ICACHE_FLASH_ATTR txbuf_write_rtc(bool rtc_only) {
   write_rtc(txmsg->rtc, rtc_only);
}
void ICACHE_FLASH_ATTR txbuf_write_flags(uint8_t flags, uint8_t mask) {
  write_flags(txmsg->rtc, flags, mask);
}

void ICACHE_FLASH_ATTR write_lastline(fer_sender_basic *fsb, uint8_t d[bytesPerPrgLine]) {
	d[0] = 0x00;
	d[1] = fsb->data[fer_dat_ADDR_2];
	d[2] = fsb->data[fer_dat_ADDR_1];
	d[3] = fsb->data[fer_dat_ADDR_0];
	d[4] = 0x53;
	d[5] = 0x00;
	d[6] = 0x10;
	d[7] = 0x11;
	d[8] = 0x05;
}

void ICACHE_FLASH_ATTR txbuf_write_lastline(fer_sender_basic *fsb) {
	write_lastline(fsb, txmsg->last);
}    


#if TEST_MODULE_FER_PRG

uint8_t test_cmd[] =
	{ 0x80, 0x49, 0x5d, 0x68, 0x2f, 0xbd, };

uint8_t test_prg[18][9] =
	{
		{ 0x32, 0x51, 0x01, 0x04, 0x14, 0x08, 0x02, 0x86, 0xa6 },
		{ 0x30, 0x09, 0x00, 0x05, 0x15, 0x09, 0x00, 0x05, 0xab }, /*ad*/
		{ 0x15, 0x09, 0x00, 0x05, 0x15, 0x09, 0x00, 0x05, 0xa0 },
		{ 0x15, 0x09, 0x00, 0x05, 0x15, 0x09, 0x00, 0x05, 0x86 },
		{ 0x30, 0x09, 0x00, 0x05, 0xff, 0x0f, 0xff, 0x0f, 0x66 },
		{ 0x34, 0x15, 0x36, 0x15, 0x36, 0x15, 0x38, 0x15, 0xf8 },
		{ 0x40, 0x15, 0x44, 0x15, 0x48, 0x15, 0x52, 0x15, 0x62 },
		{ 0x58, 0x15, 0x04, 0x16, 0x10, 0x16, 0x16, 0x16, 0x9d },
		{ 0x24, 0x16, 0x30, 0x16, 0x38, 0x16, 0x46, 0x16, 0x64 },
		{ 0x52, 0x16, 0x00, 0x17, 0x08, 0x17, 0x18, 0x17, 0x95 },
		{ 0x26, 0x17, 0x34, 0x17, 0x42, 0x17, 0x50, 0x17, 0x72 },
		{ 0x58, 0x17, 0x06, 0x18, 0x16, 0x18, 0x24, 0x18, 0xdb },
		{ 0x32, 0x18, 0x42, 0x18, 0x50, 0x18, 0x58, 0x18, 0x32 },
		{ 0x08, 0x1f, 0x16, 0x1f, 0x24, 0x1f, 0x32, 0x1f, 0x54 },
		{ 0x40, 0x1f, 0x46, 0x1f, 0x54, 0x1f, 0x00, 0x20, 0xff },
		{ 0x06, 0x20, 0x10, 0x20, 0x14, 0x20, 0x18, 0x20, 0xc0 },
		{ 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x80 },
		{ 0x00, 0x80, 0x49, 0x5d, 0x53, 0x00, 0x10, 0x00, 0x05 },

	};

const uint8_t testdat_wtimer[] =
	{ 0x56, 0x06, 0x45, 0x19, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x12, 0x07, 0x34, 0x21, 0x56,
			0x06, 0x45, 0x19, 0x0 };
const uint8_t testdat_dtimer[] = { };
extern const uint8_t astro_data[12][8];

#include "config.h"
#if 1
bool ICACHE_FLASH_ATTR testModule_fer_prg() {
	rtc_set_by_string("2017-09-07T19:55:00");

	static fer_sender_basic fsb_, *fsb = &fsb_;

	FSB_PUT_DEVID(fsb, C.fer_centralUnitID);
	FSB_PUT_GRP(fsb, 2);
	FSB_PUT_MEMB(fsb, 2);
	FSB_PUT_CMD(fsb, fer_cmd_Program);
	fer_update_tglNibble(fsb);

	write_rtc(tbuf->rtc, true);
	fer_send_prg(fsb);
	return true;
}
#else
bool ICACHE_FLASH_ATTR testModule_fer_prg()
{
	bool result;
	uint8_t group = 3, memb = 1;
	fer_cmd cmd = fer_cmd_Program;
	fer_sender_basic fsb_, *fsb = &fsb_;

	FSB_PUT_DEVID(fsb, C.fer_centralUnitID);
	FSB_PUT_GRP(fsb, group);
	FSB_PUT_MEMB(fsb, memb);
	FSB_PUT_CMD(fsb, cmd);

	uint8_t cmd_checksum; //= fer_make_cmdPacket(fsb->data, dtSendCmd);

	fer_prg_create_checksums(test_prg, cmd_checksum);
	result = fer_prg_verfiy_checksums(test_prg, cmd_checksum);

	//init_prgData((test_prg);

	write_rtc(&test_prg[FPR_RTC_START_ROW][0], false);
	write_wtimer(&test_prg[FPR_WEEKLY_START_ROW], testdat_wtimer);
	write_dtimer(&test_prg[FPR_DAILY_START_ROW][FPR_DAILY_START_COL], testdat_wtimer);
	write_astro(&test_prg[FPR_ASTRO_START_ROW], 0);

	fer_prg_create_checksums(test_prg, 0xbd);
	result = result && fer_prg_verfiy_checksums(test_prg, 0xbd);

	return result;
}
#endif

#endif // self test
