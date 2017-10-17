/* -*- compile-command: "/cygdrive/e/WinAVR-20100110/utils/bin/make.exe -C default  all && bash -c ./__flash.sh"; -*- */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#include "rtc.h"


#include "all.h"

extern fer_sender_basic default_sender;
extern fer_sender_basic last_received_sender;


static void ICACHE_FLASH_ATTR
setup_1() {
	int i;

	for (i = 1; i < 10; ++i) {
		fer_init_sender(&senders[i], 0x105dc5 - i);
	}
	fer_init_sender(&default_sender, C.fer_centralUnitID);

}



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
	if (has_cmdReceived) {
		const uint8_t *buf = get_recvCmdBuf();

		for (i=0; i < 5; ++i ) {
			last_received_sender.data[i] = buf[i];
		}

		io_puts("R:");
		frb_printPacket(buf);
		{ extern uint8_t preBitTicks; io_puts("preBitTicks: "), io_print_dec_16(preBitTicks, false), io_puts("\n"); }
		fer_recvClearAll();
	}

	if (has_prgReceived) {
		int i;
		io_puts("prgStored!\n");
		frb_printPacket(get_recvCmdBuf());

		int used_lines = FRB_GET_FPR0_IS_RTC_ONLY(get_recvPrgBufLine(FPR_RTC_START_ROW)) ? FER_RTC_PACK_CT : FER_PRG_PACK_CT;
used_lines = FER_PRG_PACK_CT;
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
		{ extern uint8_t preBitTicks; io_puts("preBitTicks: "), io_print_dec_16(preBitTicks, false), io_puts("\n"); }
		fer_recvClearAll();
	}
#endif

#ifdef FER_NTP
	ntp_update_system_time(SECS_PER_DAY);
#endif
}

int ICACHE_FLASH_ATTR
main_setup() {

	rtc_setup();
	setup_1();


#ifdef DEBUG
	test_modules();
#endif

	db_test_all_indicators(3);

	io_puts("main program starting ...\n" "build-date: " __DATE__ " " __TIME__ "\n");
	io_puts("C.fer_centralUnitID: "), io_print_hex_32(C.fer_centralUnitID, false), io_puts("\n");
	io_puts("C.mcu_serialBaud: "), io_print_dec_32(C.mcu_serialBaud, false), io_puts("\n");

	dbg_trace();
	return 0;

}

#ifdef TEST_MODULE_FERNOTRON

bool testModule_fernotron ()
{

}
#endif  // self test
