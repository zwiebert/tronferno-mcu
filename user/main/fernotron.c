/* -*- compile-command: "/cygdrive/e/WinAVR-20100110/utils/bin/make.exe -C default  all && bash -c ./__flash.sh"; -*- */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../user_config.h"
#include "common.h"
#include "current_state.h"

#include "rtc.h"

#include "all.h"
#include "set_endpos.h"
#include "timer_state.h"

extern fer_sender_basic default_sender;
extern fer_sender_basic last_received_sender;

bool  cu_auto_set(unsigned init_seconds);
bool  ntp_update_system_time(unsigned interval_seconds);

void ICACHE_FLASH_ATTR
loop(void) {
  fers_loop();

#if ENABLE_SET_ENDPOS
  sep_loop();
#endif

  cli_loop();

  timer_state_loop();

#ifdef FER_RECEIVER
  if (MessageReceived != MSG_TYPE_NONE) {
    switch (MessageReceived) {
    case MSG_TYPE_PLAIN:
      memcpy(&last_received_sender.data, rxmsg->cmd, 5);
      cu_auto_set(0);
      io_puts("R:"), fmsg_print(rxmsg, MessageReceived);
      { //TODO: improve shutter states
      //  fer_sender_basic *fsb = &last_received_sender;
       // set_shutter_state(FSB_GET_DEVID(fsb), FSB_GET_GRP(fsb), FSB_GET_MEMB(fsb), FSB_GET_CMD(fsb));
        set_shutter_state(FRB_GET_DEVID(rxmsg->cmd), FRB_GET_GRP(rxmsg->cmd), FRB_GET_MEMB(rxmsg->cmd), FRB_GET_CMD(rxmsg->cmd));

      }

      break;
#ifndef	FER_RECEIVER_MINIMAL
    case MSG_TYPE_RTC:
    case MSG_TYPE_TIMER:
      io_puts("timer frame received\n"), fmsg_print(rxmsg, MessageReceived);
      break;
#endif
    }

    if (!fmsg_verify_checksums(rxmsg, MessageReceived)) {
      io_puts("checksum(s) wrong\n");
    }

    frx_clear();
  }
#endif

#ifdef USE_NTP
  ntp_update_system_time(SECS_PER_DAY);
#endif
}




void ICACHE_FLASH_ATTR
print_startup_info(void) {
#define cfg "config "
#define slf ";\n"

static const char msg_slf[] PROGMEM = ";\n";

static const char msg_starting[] PROGMEM = "\n\n" "Tronferno-MCU starting ...\n" "build-date: " __DATE__ " " __TIME__ "\n\n";
static const char msg_hint[] PROGMEM = "\n(hint: type help; to get a command list)\n";

static const char msg_cu[] PROGMEM = cfg "cu=";
static const char msg_baud[] PROGMEM = " baud=";

static const char msg_longitude[] PROGMEM = cfg "longitude=";
static const char msg_latitude[] PROGMEM = " latitude=";
static const char msg_timezone[] PROGMEM = " time-zone=";

  if (C.app_verboseOutput >= vrbNone) {
    io_puts_P(msg_starting);
    io_puts_P(msg_cu), io_print_hex_32(C.fer_centralUnitID, false),
    io_puts_P(msg_baud), io_print_dec_32(C.mcu_serialBaud, false), io_puts_P(msg_slf);

    io_puts_P(msg_longitude), io_print_float(C.geo_longitude, 5),
    io_puts_P(msg_latitude), io_print_float(C.geo_latitude, 5),
    io_puts_P(msg_timezone),io_print_float(C.geo_timezone, 2),
#if  POSIX_TIME
        io_puts(" tz="), io_puts(C.geo_tz),
#endif
        io_puts_P(msg_slf);

#ifdef USE_WLAN
    io_puts(cfg "wlan-ssid=\""), io_puts(C.wifi_SSID), io_puts("\" wlan-password="), io_puts((C.wifi_password[0] == '\0') ? "\"\""slf : "\"***********\"" slf);
#endif
    io_puts_P(msg_hint);
  }
}

int ICACHE_FLASH_ATTR
main_setup() {

  rtc_setup();
  fer_init_sender(&default_sender, C.fer_centralUnitID);
#ifdef DEBUG
  test_modules();
#endif

  print_startup_info();
  db_test_all_indicators(3);

  dbg_trace();
  return 0;

}

#ifdef TEST_MODULE_FERNOTRON

bool testModule_fernotron ()
{

}
#endif  // self test
