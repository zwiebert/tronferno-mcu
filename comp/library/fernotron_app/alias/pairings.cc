#include <string.h>


#include "fernotron/auto/fau_tminutes.h"
#include "fernotron_trx/fer_trx_c_api.h"
#include "utils_misc/bcd.h"
#include "fernotron/alias/pairings.h"
#include "fernotron_uout/fer_uo_publish.h"
#include "debug/dbg.h"
#include "utils_misc/int_types.h"
#include <utils_time/run_time.h>
#include "main_loop/main_queue.hh"

#ifdef CONFIG_FERNOTRON_APP_DEBUG
#define DEBUG
#define D(x) x
#define DP(x) (ets_printf("%s: %s\n", logtag, x))
#define DL (ets_printf("%s:line: %d\n", logtag, (int) __LINE__))
#else
#define D(x)
#define DP(x)
#define DL
#endif
#define logtag "ferno.app.alias"

static bool pras_active;
static time_t end_time;
static uint8_t pras_g, pras_m;
static fer_alias_cmds pras_c;

static void fer_alias_enable_disable_cb(bool enable) {
  static void *tmr;

  if (enable && !tmr) {
    tmr = mainLoop_callFunByTimer(fer_alias_auto_set_check_timeout, 100, true);
  }
  if (!enable && tmr) {
    mainLoop_stopFun(tmr);
    tmr = nullptr;
  }
}

static inline void fer_alias_ENABLE_cb() {
  fer_alias_enable_disable_cb(true);
}
static inline void fer_alias_DISABLE_cb() {
  fer_alias_enable_disable_cb(false);
}


bool  fer_alias_auto_set(uint8_t g, uint8_t m, fer_alias_cmds c, uint16_t id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;

  if (timeout_secs > 0) {
    pras_g = g;
    pras_m = m;
    pras_c = c;
    end_time = run_time_s() + timeout_secs;
    pras_active = true;
    uoApp_publish_fer_prasState({ .scanning = true, .pairing = pras_c == PC_pair });
    fer_alias_ENABLE_cb();
  }
  return false;
}

void fer_alias_auto_set_check_timeout(void) {
  if (end_time == 0)
    return;

  if (end_time < run_time_s()) {
    end_time = 0;
    uoApp_publish_fer_prasState({.timeout = true});
    pras_active = false;
    fer_alias_DISABLE_cb();
  }
}

bool fer_alias_auto_set_check(const uint32_t a) {
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
    uoApp_publish_fer_prasState({.a=a, .success = success, .pairing = pras_c == PC_pair });
    end_time = 0;

    pras_active = false;
    fer_alias_DISABLE_cb();
    return true;
  }
  return false;
}


