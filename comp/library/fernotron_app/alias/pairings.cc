#include <string.h>

#include "app_config/proj_app_cfg.h"
#include "fernotron/auto/fau_tminutes.h"
#include "fernotron_trx/fer_trx_c_api.h"
#include "utils_misc/bcd.h"
#include "fernotron/alias/pairings.h"
#include "app_uout/status_output.h"
#include "app_uout/callbacks.h"
#include "debug/dbg.h"
#include "utils_misc/int_types.h"
#include <utils_time/run_time.h>

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
static u8 pras_g, pras_m;
static fer_alias_cmds pras_c;

static inline void fer_alias_ENABLE_cb() {
  if (fer_alias_enable_disable_cb)
    fer_alias_enable_disable_cb(true);
}
static inline void fer_alias_DISABLE_cb() {
  if (fer_alias_enable_disable_cb)
    fer_alias_enable_disable_cb(false);
}

bool  fer_alias_auto_set(const struct TargetDesc &td, u8 g, u8 m, fer_alias_cmds c, u16 id, unsigned timeout_secs) {
  if (end_time != 0)
    return false;

  if (timeout_secs > 0) {
    pras_g = g;
    pras_m = m;
    pras_c = c;
    end_time = run_time_s() + timeout_secs;
    pras_active = true;
    uoApp_publish_fer_prasState({ .scanning = true, .pairing = pras_c == PC_pair });
    soMsg_pras_start_listening(td, id);
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
    uoApp_publish_fer_prasState({.a=a, .success = success, .pairing = pras_c == PC_pair });
    end_time = 0;

    pras_active = false;
    fer_alias_DISABLE_cb();
    return true;
  }
  return false;
}


