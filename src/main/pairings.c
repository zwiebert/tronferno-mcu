#include <string.h>

#include "user_config.h"

#include "automatic/timer_state.h"
#include "cli/cli.h"

#include "misc/bcd.h"
#include "main/pairings.h"

#include "../motor_setup/set_endpos.h"
#include "../shutter_positions/current_state.h"
#include "main/rtc.h"
#include "../userio/status_output.h"

#if defined DISTRIBUTION || 0
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif


static bool pras_active;
static time_t end_time;
static u8 pras_g, pras_m, pras_c;

bool  pair_auto_set(u8 g, u8 m, u8 c, u16 id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;

  if (timeout_secs > 0) {
    pras_g = g;
    pras_m = m;
    pras_c = c;
    end_time = run_time(NULL) + timeout_secs;
    last_received_sender.data[0] = 0;
    pras_active = true;
    so_output_message(SO_PRAS_START_LISTENING, &id);
  }
  return false;
}

bool  pair_auto_set_check_timeout(void) {
  if (end_time == 0)
    return false;

  if (end_time < run_time(NULL)) {
    end_time = 0;
    so_output_message(SO_PRAS_STOP_LISTENING, NULL);
    so_output_message(SO_PRAS_TIMEOUT, NULL);
    pras_active = false;
    return true;
  }
  return false;
}

bool  pair_auto_set_check(fer_sender_basic *fsb) {
  if (end_time == 0)
    return false;

  if (!FSB_ADDR_IS_CENTRAL(fsb)) {
    u32 a = FSB_GET_DEVID(fsb);
    bool success = false;

    if (pras_c == PC_pair || pras_c == PC_unpair) {
      DL;
      if (pair_controller(a, pras_g, pras_m, pras_c == PC_unpair)) {
        DL;
        success = true;
      }
    }

    int result = success ? pras_c : -pras_c;

    so_output_message(SO_PRAS_DONE, &result);
    end_time = 0;

    pras_active = false;
    return true;
  }
  return false;
}


