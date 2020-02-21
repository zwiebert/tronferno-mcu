/*
 * commands.c
 *
 *  Created on: 09.02.2020
 *      Author: bertw
 */

#include "fernotron_pos/commands.h"
#include "fernotron_pos/current_state.h"
#include "app/proj_app_cfg.h"
#include "fernotron/fer_rx_tx.h"
#include "cli_app/cli_imp.h"
#include "cli_app/cli_fer.h"

bool   commands_sendShutterCommand(u32 a, u8 g, u8 m, fer_cmd cmd, u8 repeats) {
  fsbT *fsb = get_fsb(a,g,m,cmd);
  return fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
}

bool   commands_moveShutterToPct(u32 a, u8 g, u8 m, u8 pct, u8 repeats) {
  precond(g <= 7 && m <= 7 && pct <= 100);
  int curr_pct = -1;

  fer_cmd fc = fer_cmd_None;
  fer_grp group = g;
  fer_memb memb = m == 0 ? 0 : m + 7;

  if (g > 0 && m > 0) {
    curr_pct = currentState_getShutterPct(a, g, m);
  }

  fsbT *fsb = get_sender_by_addr(a);
  if (!fsb) {
    static fsbT fsb_direct; // FIXME: or was senders[0] meant for this?
    fsb = &fsb_direct;
    if (FSB_GET_DEVID(fsb) != a) {
      fer_init_sender(fsb, a);
    }
  }

  if (FSB_ADDR_IS_CENTRAL(fsb)) {
    FSB_PUT_GRP(fsb, group);
    FSB_PUT_MEMB(fsb, memb);
  }

  if (pct == 0 || pct == 100) {
    fc = (pct == 0) ? fer_cmd_DOWN : fer_cmd_UP;
    FSB_PUT_CMD(fsb, fc);
    fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
  } else if (curr_pct >= 0) {
    u16 stop_delay = currentState_mvCalcTime10(g, m, curr_pct, pct);
    if (stop_delay == 0)
      return false;
    fc = (pct < curr_pct) ? fer_cmd_DOWN : fer_cmd_UP;
    FSB_PUT_CMD(fsb, fc);
    fer_send_msg_with_stop(fsb, 0, stop_delay, repeats);
  } else {
    FSB_PUT_CMD(fsb, fer_cmd_UP);
    fer_send_msg(fsb, MSG_TYPE_PLAIN, repeats);
    u16 delay = currentState_mvCalcTime10(g, m, 0, 100);
    u16 stop_delay = currentState_mvCalcTime10(g, m, 100, pct);
    if (stop_delay == 0)
      return false;
    FSB_PUT_CMD(fsb, fer_cmd_DOWN);
    fer_send_msg_with_stop(fsb, delay, stop_delay, repeats);
  }

  return false;
}
