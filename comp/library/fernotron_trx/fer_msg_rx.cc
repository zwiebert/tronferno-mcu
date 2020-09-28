#include "fer_app_cfg.h"
#include "fernotron/fer_msg_type.h"
#include "fernotron/fer_msg_rx.h"
#include "fernotron/fer_rawmsg_buffer.h"
#include "fernotron/trx/timer_data.h"
#include "debug/dbg.h"
#include <string.h>

#include <fernotron/trx/fer_trx_incoming.hh>
#include "fer_trx_incoming_event.hh"

fer_sbT last_received_sender;

void fer_rx_loop() {
#ifdef FER_RECEIVER
  if (fer_rx_messageReceived != MSG_TYPE_NONE) {
    Fer_Trx_IncomingEvent evt {};
    evt.kind  = fer_rx_messageReceived;
    evt.raw = fer_rx_msg;
    memcpy(&evt.fsb, fer_rx_msg->cmd.bd, 5);

    if (evt.kind == MSG_TYPE_PLAIN) {
      if (0 == memcmp(&last_received_sender.data, fer_rx_msg->cmd.bd, 5)) {
        evt.kind = MSG_TYPE_PLAIN_DOUBLE;
      } else {
        memcpy(&last_received_sender.data, fer_rx_msg->cmd.bd, 5);
        last_received_sender = evt.fsb;
      }
    }

    if (fer_msg_raw_checksumsVerify(fer_rx_msg, fer_rx_messageReceived)) {
      Fer_Trx_IncomingMsg::push_event(&evt);
      fer_rx_clear();
    }
  }
#endif
}
