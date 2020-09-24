/*
 * commands.c
 *
 *  Created on: 09.02.2020
 *      Author: bertw
 */

#include "fernotron/fsb.h"
#include "fernotron/fer_main.h"
#include "fernotron/fer_msg_plain.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/pos/commands.h"
#include "fernotron/pos/shutter_pct.h"
#include "app/config/proj_app_cfg.h"
#include "app/settings/config.h"
#include "debug/dbg.h"
#include <fernotron/api/fer_msg_send.hh>

#define GRP_COUNT 7
#define MBR_COUNT 7
#define GRP_MAX 7
#define MBR_MAX 7

bool   fer_cmd_sendShutterCommand(u32 a, u8 g, u8 m, fer_cmd cmd, u8 repeats) {
  Fer_MsgCmd msg { .a = a, .g = g, .m = m, .cmd = cmd, .repeats = repeats };
  return fer_api_tx.send(msg);
}

bool fer_cmd_moveShutterToPct(u32 a, u8 g, u8 m, u8 pct, u8 repeats) {
  precond(g <= 7 && m <= 7 && pct <= 100);
  int curr_pct = -1;

  bool is_our_cu = a == cfg_getCuId();
  bool is_cu = FER_ADDR_TYPE_CentralUnit == (a >> 16);

  if (!is_cu && (g || m))
    return false;

  if (is_cu && m == 0) {
    Fer_GmBitMask gm;
    gm[g] = 0xfe;
    return fer_cmd_moveShuttersToPct(a, &gm, pct, repeats);
  }

  fer_cmd fc = fer_cmd_None;
  fer_grp group = fer_ngrp2fer_grp(g);
  fer_memb memb = fer_nmb2fer_memb(m);

  if (is_our_cu && g > 0 && m > 0) {
    curr_pct = fer_statPos_getPct(a, g, m);
    io_printf_v(vrbDebug, "curr_pct: %d\n", curr_pct);
  }

  fer_sbT *fsb = fer_main_getSenderByAddress(a);
  if (!fsb) {
    static fer_sbT fsb_direct; // FIXME: or was senders[0] meant for this?
    fsb = &fsb_direct;
    if (FER_SB_GET_DEVID(fsb) != a) {
      fer_init_sender(fsb, a);
    }
  }

  if (FER_SB_ADDR_IS_CENTRAL(fsb)) {
    FER_SB_PUT_GRP(fsb, group);
    FER_SB_PUT_MEMB(fsb, memb);
  }

  if (pct == 0 || pct == 100) {
    fc = (pct == 0) ? fer_cmd_DOWN : fer_cmd_UP;
    FER_SB_PUT_CMD(fsb, fc);
    return fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
  }

  if (!is_our_cu)
    return false;

  if (curr_pct >= 0) {
    u16 stop_delay = fer_simPos_calcMoveDuration_fromPctDiff_m(g, m, curr_pct, pct);
    if (stop_delay == 0)
      return false;
    fc = (pct < curr_pct) ? fer_cmd_DOWN : fer_cmd_UP;
    FER_SB_PUT_CMD(fsb, fc);
    return fer_send_msg_with_stop(fsb, 0, stop_delay, repeats);
  }

  {
    FER_SB_PUT_CMD(fsb, fer_cmd_UP);
    fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
    u16 delay = fer_simPos_calcMoveDuration_fromPctDiff_m(g, m, 0, 100);
    u16 stop_delay = fer_simPos_calcMoveDuration_fromPctDiff_m(g, m, 100, pct);
    if (stop_delay == 0)
      return false;
    FER_SB_PUT_CMD(fsb, fer_cmd_DOWN);
    return fer_send_msg_with_stop(fsb, delay, stop_delay, repeats);
  }

  return true;
}

bool   fer_cmd_moveShuttersToPct(u32 a, Fer_GmBitMask *gm, u8 pct, u8 repeats) {
  u8 g, m;

  for (g=1; g <= GRP_MAX; ++g) {
    u8 um = (C.fer_usedMembers >> (g*4)) & 0x0f;
    for (m=1; m <= MBR_MAX; ++m) {
      if (m > um)
        break;
      if (!gm->getBit(g, m))
        continue;

      if (!fer_cmd_moveShutterToPct(a, g, m, pct, 0)) {
        return false;
      }
    }
  }
  return true;
}
