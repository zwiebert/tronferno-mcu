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
  fer_sbT fsb;
  uint32_t s10; // time in tenth of secs
  fer_msg_type mt : 3;
  int16_t repeats : 4;
  int16_t sent_ct : 5;
};


struct sf *fer_tx_nextMsg();
void fer_tx_popMsg();
bool fer_tx_pushMsg(const struct sf *msg);
int fer_tx_get_msgPendingCount();

