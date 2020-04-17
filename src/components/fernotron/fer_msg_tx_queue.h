/*
 * fer_msg_tx_queue.h
 *
 *  Created on: 15.04.2020
 *      Author: bertw
 */

#ifndef COMPONENTS_FERNOTRON_FER_MSG_TX_QUEUE_H_
#define COMPONENTS_FERNOTRON_FER_MSG_TX_QUEUE_H_

#include "fernotron/fsb.h"
#include <stdint.h>
#include <stdbool.h>

struct sf {
  fsbT fsb;
  uint32_t s10; // time in tenth of secs
  uint16_t /*fmsg_type*/ mt : 3;
  uint16_t repeats : 4;
  uint16_t sent_ct : 5;
};


struct sf *ftx_nextMsg();
void ftx_popMsg();
bool ftx_pushMsg(const struct sf *msg);
int ftx_get_msgPendingCount();

#endif /* COMPONENTS_FERNOTRON_FER_MSG_TX_QUEUE_H_ */
