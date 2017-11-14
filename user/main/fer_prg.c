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


#if BUFFER_SHARING
	struct fer_msg message_buffer;
#endif


/* verify checksums of data word array */
bool ICACHE_FLASH_ATTR fer_cmd_verify_checksum(ferCmdBuf_type dg, uint8_t *checksum_out) {
	uint8_t i, checksum = 0;

	for (i = 0; i < bytesPerCmdPacket - 1; ++i) {
		checksum += dg[i];
	}
	if (dg[i] == checksum) {
		if (checksum_out)
			*checksum_out = checksum;
		return true;
	}

	return false;

}

/* verify checksums in prg byte array. parameter checksum is the starting value (the checksum of the leading data packet) */
bool ICACHE_FLASH_ATTR fer_prg_verfiy_checksums(uint8_t dg[linesPerPrg][bytesPerPrgLine], uint8_t checksum) {
	int line, column;
	checksum *= 2;

	for (line = 0; line < linesPerPrg - 1; ++line) {
		for (column = 0; column < bytesPerPrgLine; ++column) {

			if (column == bytesPerPrgLine - 1 && dg[line][column] != checksum) {
				return false;
			}

			checksum += dg[line][column];

		}
	}
	return true;
}

/* create checksums of prg byte array in place. parameter checksum is the starting value (the checksum of the leading data packet) */
uint8_t ICACHE_FLASH_ATTR fer_prg_create_checksums(uint8_t dg[linesPerPrg][bytesPerPrgLine], uint8_t checksum) {
	int line, column;
	checksum *= 2; // add the previous checksum to all sums ... should this better be done by the caller?

	for (line = 0; line < linesPerPrg - 1; ++line) {
		for (column = 0; column < bytesPerPrgLine; ++column) {

			if (column == bytesPerPrgLine - 1) {
				dg[line][column] = checksum;
			}

			checksum += dg[line][column];
		}
	}
	return checksum;
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

void ICACHE_FLASH_ATTR init_prgPacket(uint8_t d[linesPerPrg][bytesPerPrgLine]) {
	int col, line;

	for (col = 0; col < FPR_RTC_WIDTH; ++col) {
		d[FPR_RTC_START_ROW][col] = 0x00;
	}

	for (line = FPR_TIMER_START_ROW; line < (FPR_TIMER_START_ROW + FPR_TIMER_HEIGHT + FPR_ASTRO_HEIGHT); ++line) {
		for (col = 0; col < FPR_TIMER_WIDTH; ++col) {
			d[line][col] = 0xff;
			++col;
			d[line][col] = 0x0f;
		}
	}
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
  write_wtimer(tbuf->wdtimer, wtimer_data);
}


void ICACHE_FLASH_ATTR txbuf_write_astro(int mint_offset) {
  write_astro(tbuf->astro, mint_offset);
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
   write_rtc(tbuf->rtc, rtc_only);
}
void ICACHE_FLASH_ATTR txbuf_write_flags(uint8_t flags, uint8_t mask) {
  write_flags(tbuf->rtc, flags, mask);
}

void ICACHE_FLASH_ATTR write_lastline(fer_sender_basic *fsb, uint8_t d[FPR_ASTRO_WIDTH]) {
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
	write_lastline(fsb, get_sendPrgBufLine(FPR_LAST_START_ROW));
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

	write_rtc(get_sendPrgBufLine(FPR_RTC_START_ROW), true);
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

	init_prgPacket(test_prg);

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
