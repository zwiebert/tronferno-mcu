#include "fer_app_cfg.h"
#include "fernotron_trx/raw/fer_radio_trx.h"
#include "fernotron_trx/raw/fer_msg_type.h"
#include "fernotron_trx/fer_trx_c_api.h"
#include "fernotron_trx/raw/fer_rawmsg_buffer.h"
#include "fernotron_trx/raw/fer_rawmsg_build.h"
#include "fernotron_trx/timer_data.h"
#include "debug/dbg.h"
#include <string.h>

#include <fernotron_trx/fer_trx_api.hh>
#include "fer_trx_incoming_event.hh"

static bool ferCmd_isEqual(const Fer_Cmd a, const Fer_Cmd b) {
#if 0
  if (0 != memcmp(&a,&b, sizeof a)) {
     return false;
  }
#else
  if (a.addr[0] != b.addr[0] || a.addr[1] != b.addr[1] || a.addr[2] != b.addr[2])
    return false;

  if (a.cmd != b.cmd)
    return false;

  if (a.grp != b.grp)
    return false;

  if (a.memb != b.memb)
    return false;

  if (a.tgl != b.tgl)
    return false;
#endif
  return true;

}


void fer_rx_loop() {
#ifdef FER_RECEIVER
  static fer_sbT last_received_sender;

  if (fer_rx_messageReceived != MSG_TYPE_NONE) {
    Fer_Trx_IncomingEvent evt {};
    evt.kind  = fer_rx_messageReceived;
    evt.raw = fer_rx_msg;
    memcpy(&evt.fsb, fer_rx_msg->cmd.bd, 5);

    if (evt.kind == MSG_TYPE_PLAIN) {
      if (ferCmd_isEqual(last_received_sender.sd, fer_rx_msg->cmd.sd.cmd)) {
        evt.kind = MSG_TYPE_PLAIN_DOUBLE;
      } else {
        memcpy(&last_received_sender.data, fer_rx_msg->cmd.bd, 5);
        last_received_sender = evt.fsb;
      }
    }

    if (fer_msg_raw_checksumsVerify(fer_rx_msg, fer_rx_messageReceived)) {
      Fer_Trx_API::push_event(&evt);
      fer_rx_clear();
    }
  }
#endif
}
