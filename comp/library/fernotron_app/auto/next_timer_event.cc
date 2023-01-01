#include "app_config/proj_app_cfg.h"

#include <fernotron/auto/fau_next_event.hh>
#include <fernotron_trx/fer_trx_api.hh>
#include <fernotron/pos/positions_dynamic.h>

#include "time.h"

Fer_TimerEvent next_event_te;

void fer_am_loop(void) {
  const time_t now_time = time(NULL);
  if (next_event_te.te_isEventNow(now_time)) {
    fer_simPos_registerMovingShutters(next_event_te.te_getMaskUp(), fer_if_cmd_UP);
    fer_simPos_registerMovingShutters(next_event_te.te_getMaskDown(), fer_if_cmd_DOWN);
    next_event_te.fer_am_updateTimerEvent(now_time);
  }
}

