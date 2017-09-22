/* -*- compile-command: "/cygdrive/e/WinAVR-20100110/utils/bin/make.exe -C default  all && bash -c ./__flash.sh"; -*- */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#ifdef AVR_TIME
#include <time.h>
//#include <util/eu_dst.h>
#include <util/usa_dst.h>
#else
#include "../time/time.h"
#endif

int16_t ICACHE_FLASH_ATTR
eu_dst(const time_t *timer, int32_t * z) {
	uint32_t t = *timer;
	if ((uint8_t) (t >> 24) >= 194)
		t -= 3029443200U;
	t = (t + 655513200) / 604800 * 28;
	if ((uint16_t) (t % 1461) < 856)
		return 3600;
	else
		return 0;
}

#include "all.h"

extern void setup_mcu(void);

extern fer_sender_basic default_sender;

extern fer_sender_basic last_received_sender;


static void ICACHE_FLASH_ATTR
setup_1() {
	int i;

	set_zone(ONE_HOUR); //* C.timezone);
	set_position(C.latitude * ONE_DEGREE, C.longitude * ONE_DEGREE);

	switch (C.dST) {

	case dstEU:
		set_dst(eu_dst);
		break;
#ifdef AVR_TIME
		case dstUS:
		set_dst(usa_dst);
		break;
#endif

	case dstNone:
	default:
		set_dst(NULL);
		break;
	}

	for (i = 1; i < 10; ++i) {
		fer_init_sender(&senders[i], 0x105dc5 - i);
	}
	fer_init_sender(&default_sender, C.centralUnitID);

}

#ifdef TEST_MODULE_FERNOTRON

bool testModule_fernotron ()
{

}
#endif  // self test


void ICACHE_FLASH_ATTR
loop(void) {
	int i;
	char *cmdline;

	// io_printf_fun("loop()\n");
	// CLI
	if ((cmdline = get_commandline())) {
		io_puts("cmd :");
		io_puts(cmdline);
		io_puts("\n");
		process_cmdline(cmdline);
	}

#ifdef FER_RECEIVER
	// received data
	extern bool is_recPrgFrame;
	if (has_cmdReceived) {
		const uint8_t *buf = get_recvCmdBuf();

		for (i=0; i < 5; ++i ) {
			last_received_sender.data[i] = buf[i];
		}

		io_puts("R:");
		frb_printPacket(buf);

		CountRecvErrors = 0;
		has_cmdReceived = false;


	}

	db_put_light(is_recPrgFrame, 1);  // debug LEDs

	db_put_light(has_prgReceived, 2);

	if (has_prgReceived) {
		int i;
		io_puts("prgStored!\n");
		frb_printPacket(get_recvCmdBuf());

		int used_lines = FRB_GET_FPR0_IS_RTC_ONLY(get_recvPrgBufLine(FPR_RTC_START_ROW)) ? FER_RTC_PACK_CT : FER_PRG_PACK_CT;

		for (i=0; i < used_lines; ++i) {
			print_array_8(get_recvPrgBufLine(i), FER_PRG_BYTE_CT);
		}

		uint8_t cs;
		if (fer_cmd_verify_checksum(get_recvCmdBuf(), &cs)) {
			if (fer_prg_verfiy_checksums(dtRecvPrgFrame, cs)) {
				io_puts("checksum ok\n");
			}
		}

#ifndef DB_SHORT
		fpr_printPrgPacketInfo(dtRecvPrgFrame, used_lines == 1);
#endif

		CountRecvErrors = 0;
		has_prgReceived = false;
	}
#endif

}

extern bool test();

int ICACHE_FLASH_ATTR
main_setup() {

	setup_1();

#ifdef DEBUG
	test_modules();
#endif

	db_test_all_indicators(3);

	io_puts("main program starting ...\n" "build-date: " __DATE__ " " __TIME__ "\n");
	io_puts("C.centralUnitID: "), io_print_hex_32(C.centralUnitID, false), io_puts("\n");
	io_puts("C.serialBaud: "), io_print_dec_32(C.serialBaud, false), io_puts("\n");

	dbg_trace();
	return 0;

}

