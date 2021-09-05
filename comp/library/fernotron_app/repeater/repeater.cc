#include <fernotron/repeater/repeater.h>

#include <fernotron_trx/fer_trx_api.hh>
#include <fernotron_trx/raw/fer_msg_plain.h>
#include <fernotron_trx/raw/fer_msg_tx.h>
#include <fernotron_trx/raw/fer_fsb.h>
#include "../fer_msg_tx_queue.h"

#include <utils_time/run_time.h>

const unsigned REP_DELAY_TS = 10;

static bool ferRep_isKnownSender(const struct Fer_Cmd *cmd) {
  if (!(FER_RB_ADDR_IS_PLAIN(cmd) || FER_RB_ADDR_IS_SUNSENS(cmd)))
    return false;

  return true;
}

bool ferRep_repeatCommand(struct Fer_Cmd cmd) {
  if (!ferRep_isKnownSender(&cmd))
    return false;

  struct sf msg { .fsb = { .sd = cmd}, .when_to_transmit_ts = (u32)get_now_time_ts() + REP_DELAY_TS, .mt = MSG_TYPE_PLAIN, .repeats = 1, .sent_ct = 1 };

  fer_tx_pushMsg(&msg);
  fer_tx_loop();

  return true;
}
