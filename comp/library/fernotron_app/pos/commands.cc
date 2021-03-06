/*
 * commands.c
 *
 *  Created on: 09.02.2020
 *      Author: bertw
 */



#include "fernotron/pos/commands.h"
#include "fernotron/pos/shutter_pct.h"
#include "app_config/proj_app_cfg.h"
#include "fernotron/fer_main.h"
#include "debug/dbg.h"
#include <fernotron_trx/fer_trx_api.hh>

#define GRP_MAX 7
#define MBR_MAX 7

bool fer_cmd_sendShutterCommand(u32 a, u8 g, u8 m, fer_if_cmd cmd, u8 repeats) {
  precond(a != 0);
  Fer_MsgCmd msg { .a = a, .g = g, .m = m, .cmd = cmd, .repeats = repeats };
  return Fer_Trx_API::send_cmd(msg);
}

bool fer_cmd_moveShutterToPct(u32 a, u8 g, u8 m, u8 pct_arg, u8 repeats) {
  precond(g <= 7 && m <= 7 && pct_arg <= 100);
  precond(a != 0);

  Pct curr_pct;
  Pct pct{pct_arg};

  bool is_our_cu = a == fer_config.cu;
  bool is_cu = FER_U32_TEST_TYPE(a, FER_ADDR_TYPE_CentralUnit);

  if (!is_cu && (g || m))
    return false;

  if (is_our_cu && m == 0) {
    Fer_GmSet gm;
    gm[g] = 0xfe;
    return fer_if_cmd_moveShuttersToPct(a, &gm, pct, repeats);
  }


  if (is_our_cu && g > 0 && m > 0) {
    curr_pct = fer_statPos_getPct(g, m);
    io_printf_v(vrbDebug, "curr_pct: %d\n", curr_pct.value());
  }


  if (pct == Pct(0) || pct == Pct(100)) {
    fer_if_cmd fc = (pct == Pct(0)) ? fer_if_cmd_DOWN : fer_if_cmd_UP;
    return fer_cmd_sendShutterCommand(a, g, m, fc, repeats);
  }

  if (!is_our_cu)
    return false;

  if (curr_pct) {
    u16 stop_delay = fer_simPos_calcMoveDuration_fromPctDiff_m(g, m, curr_pct, pct);
    if (stop_delay == 0)
      return false;
    fer_if_cmd fc = (pct < curr_pct) ? fer_if_cmd_DOWN : fer_if_cmd_UP;
    Fer_MsgCmd msg { .a = a, .g = g, .m = m, .cmd = fc, .repeats = repeats, .stopDelay = stop_delay };
    return Fer_Trx_API::send_cmd(msg);
  }

  {
    fer_cmd_sendShutterCommand(a, g, m, fer_if_cmd_UP, repeats);
    u16 delay = fer_simPos_calcMoveDuration_fromPctDiff_m(g, m, 0, 100);
    u16 stop_delay = fer_simPos_calcMoveDuration_fromPctDiff_m(g, m, 100, pct);
    if (stop_delay == 0)
      return false;
    Fer_MsgCmd msg { .a = a, .g = g, .m = m, .cmd = fer_if_cmd_DOWN, .repeats = repeats, .delay = delay, .stopDelay = stop_delay };
    return Fer_Trx_API::send_cmd(msg);
  }

  return true;
}

bool fer_if_cmd_moveShuttersToPct(uint32_t a, Fer_GmSet *gm, u8 pct, u8 repeats) {

  for (auto it = gm->begin(1, 1, true); it; ++it) {
    u8 g = it.getG(), m = it.getM();

    if (!fer_cmd_moveShutterToPct(a, g, m, pct, 0)) {
      return false;
    }
  }

  return true;
}

