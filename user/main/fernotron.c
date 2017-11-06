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

void ICACHE_FLASH_ATTR fer_printData(const uint8_t *cmd, uint8_t prg[linesPerPrg][bytesPerPrgLine]) {

	if (cmd)
		frb_printPacket(cmd);
#ifndef	FER_RECEIVER_MINIMAL
	if (prg) {
		int i, used_lines;

		used_lines = FRB_GET_FPR0_IS_RTC_ONLY(prg[FPR_RTC_START_ROW]) ? FER_RTC_PACK_CT : FER_PRG_PACK_CT;
#if 0
		for (i = 0; i < used_lines; ++i) {
			print_array_8(prg[i], FER_PRG_BYTE_CT);
		}
#else
		fpr_printPrgPacketInfo(prg, used_lines == 1);
#endif
	}
#endif
}


bool ICACHE_FLASH_ATTR cu_auto_set(unsigned init_seconds);


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



		io_puts("R:"), fer_printData(buf, NULL);
		fer_recvClearAll();
	}

	cu_auto_set(0);
#ifndef	FER_RECEIVER_MINIMAL
	if (has_prgReceived) {

		io_puts("timer frame received\n"), fer_printData(get_recvCmdBuf(), dtRecvPrgFrame);

		uint8_t cs;
		if (fer_cmd_verify_checksum(get_recvCmdBuf(), &cs)) {
			if (fer_prg_verfiy_checksums(dtRecvPrgFrame, cs)) {
				io_puts("frame checksums ok\n");
			}
		}

		fer_recvClearAll();
	}
#endif
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

	if (C.app_verboseOutput >= vrbNone) {
		io_puts("\n\n" "main program starting ...\n" "build-date: " __DATE__ " " __TIME__ "\n");
		io_puts("C.fer_centralUnitID: "), io_print_hex_32(C.fer_centralUnitID, false), io_puts("\n");
		io_puts("C.mcu_serialBaud: "), io_print_dec_32(C.mcu_serialBaud, false), io_puts("\n");
		io_puts("C.geo_longitude: "), io_print_float(C.geo_longitude, 5), io_puts("\n");
		io_puts("C.geo_latitude: "), io_print_float(C.geo_latitude, 5), io_puts("\n");
		io_puts("C.geo_timezone: "), io_print_float(C.geo_timezone, 2), io_puts("\n");
#ifdef USE_WLAN
		io_puts("C.wifi_SSID: \""), io_puts(C.wifi_SSID), io_puts("\"\n");
		io_puts("C.wifi_password: \""), io_puts((C.wifi_password[0] == '\0') ? "\"\n" : "***********\"\n");
#endif
	}

	dbg_trace();
	return 0;

}

#ifdef TEST_MODULE_FERNOTRON

bool testModule_fernotron ()
{

}
#endif  // self test
