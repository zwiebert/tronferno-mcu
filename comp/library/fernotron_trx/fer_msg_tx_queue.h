/*
 * fer_msg_tx_queue.h
 *
 *  Created on: 15.04.2020
 *      Author: bertw
 */

#pragma once

#include "fernotron/trx/raw/fer_fsb.h"
#include <stdint.h>
#include <stdbool.h>

struct sf {
  fer_sbT fsb; ///< message data
  uint32_t when_to_transmit_ts; ///< run-time (in s/10) when the messages should be sent
  fer_msg_type mt : 3; ///< type/kind of message.
  int16_t repeats : 4; ///< message should be transmitted  (1 + REPEATS) times
  int16_t sent_ct : 5; ///< counter, how often the message has been transmitted already
};



struct sf *fer_tx_nextMsg();
void fer_tx_popMsg();
bool fer_tx_pushMsg(const struct sf *msg);
extern "C" int fer_tx_get_msgPendingCount();

