#include <string.h>

#include "app/config/proj_app_cfg.h"
#include "fernotron/auto/fau_tminutes.h"
#include "fernotron/fer_msg_rx.h"
#include "misc/bcd.h"
#include "fernotron/alias/pairings.h"
#include "app/rtc.h"
#include "app/uout/status_output.h"
#include "debug/dbg.h"
#include "misc/int_types.h"

#if defined DISTRIBUTION || 0
#define D(x) x
#define DP(x) (ets_printf("db: %s\n", x))
#define DL (ets_printf("db:line: %d\n", (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif

void (*fer_alias_enable_disable_cb)(bool enable);

static bool pras_active;
static time_t end_time;
static u8 pras_g, pras_m, pras_c;
static const struct TargetDesc *my_td;

static inline void fer_alias_ENABLE_cb() {
  if (fer_alias_enable_disable_cb)
    fer_alias_enable_disable_cb(true);
}
static inline void fer_alias_DISABLE_cb() {
  if (fer_alias_enable_disable_cb)
    fer_alias_enable_disable_cb(false);
}

bool  fer_alias_auto_set(const struct TargetDesc &td, u8 g, u8 m, u8 c, u16 id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;
  my_td = &td;

  if (timeout_secs > 0) {
    pras_g = g;
    pras_m = m;
    pras_c = c;
    end_time = run_time_s() + timeout_secs;
    last_received_sender.data[0] = 0;
    pras_active = true;
    soMsg_pras_start_listening(*my_td, id);
    fer_alias_ENABLE_cb();
  }
  return false;
}

void fer_alias_auto_set_check_timeout(void) {
  if (end_time == 0)
    return;

  if (end_time < run_time_s()) {
    end_time = 0;
    soMsg_pras_stop_listening(*my_td);
    soMsg_pras_timeout(*my_td);
    pras_active = false;
    fer_alias_DISABLE_cb();
  }
}

bool fer_alias_auto_set_check(const u32 a) {
  if (end_time == 0)
    return false;
  {
    bool success = false;

    if (pras_c == PC_pair || pras_c == PC_unpair) {
      DL;
      if (fer_alias_controller(a, pras_g, pras_m, pras_c == PC_unpair)) {
        DL;
        success = true;
      }
    }

    soMsg_pras_done(*my_td, success, pras_c == PC_unpair);
    end_time = 0;

    pras_active = false;
    fer_alias_DISABLE_cb();
    return true;
  }
  return false;
}


