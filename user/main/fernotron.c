#include <string.h>

#include "user_config.h"


#include "automatic/timer_state.h"
#include "cli/cli.h"
#include "config/config.h"
#include "fernotron/fer_code.h"
#include "fernotron/fer_sender.h"
#include "main/cuid_auto_set.h"
#include "main/debug.h"
#include "main/rtc.h"
#include "setup/set_endpos.h"
#include "main/pairings.h"
#include "positions/current_state.h"
#include "userio/status_output.h"

extern fer_sender_basic default_sender;
extern fer_sender_basic last_received_sender;

bool  ntp_update_system_time(unsigned interval_seconds);

void ICACHE_FLASH_ATTR
loop(void) {
  fers_loop();

#if ENABLE_SET_ENDPOS
  sep_loop();
#endif

  cli_loop();

  timer_state_loop();
  cu_auto_set_check_timeout();
#ifdef USE_PAIRINGS
        pair_auto_set_check_timeout();
#endif

#ifdef FER_RECEIVER
  if (MessageReceived != MSG_TYPE_NONE) {
    switch (MessageReceived) {
      case MSG_TYPE_PLAIN: {
        bool isDouble = (0 == memcmp(&last_received_sender.data, rxmsg->cmd, 5));
        memcpy(&last_received_sender.data, rxmsg->cmd, 5);
        io_puts("R:"), fmsg_print(rxmsg, MessageReceived);
        io_puts(isDouble ? "c:" : "C:"), fmsg_print_as_cmdline(rxmsg, MessageReceived);
        cu_auto_set_check(&last_received_sender);
#ifdef USE_PAIRINGS
        pair_auto_set_check(&last_received_sender);
#endif
        { //TODO: improve shutter states
          uint8_t g=0, m=0;

          if (FRB_ADDR_IS_CENTRAL(rxmsg->cmd)) {
            g = FRB_GET_GRP(rxmsg->cmd);
            m = FRB_GET_MEMB(rxmsg->cmd);
            if (m)
            m -= 7;
          }
          set_shutter_state(FRB_GET_DEVID(rxmsg->cmd), g, m, FRB_GET_CMD(rxmsg->cmd));
        }
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

#if defined USE_NTP && defined MCU_ESP8266
  ntp_update_system_time(SECS_PER_DAY);
#endif
}


int ICACHE_FLASH_ATTR
main_setup() {

  rtc_setup();
  fer_init_sender(&default_sender, C.fer_centralUnitID);
#ifdef DEBUG
  test_modules();
#endif

  so_output_message(SO_FW_START_MSG_PRINT, 0);
  db_test_all_indicators(3);

  dbg_trace();
  return 0;

}

#ifdef TEST_MODULE_FERNOTRON

bool testModule_fernotron ()
{

}
#endif  // self test
