/*
 * fer_msg_tx_queue.c
 *
 *  Created on: 15.04.2020
 *      Author: bertw
 */

#include "fer_msg_tx_queue.h"
#include "utils_misc/int_types.h"
#include "fernotron_trx/raw/fer_msg_tx.h"

#define sf_SIZE 16
static struct sf sf[sf_SIZE];
static u8 sf_head_, sf_tail_;

#define sf_head (sf_head_+0)
#define sf_tail (sf_tail_+0)
#define sf_incrHead() ((void)(sf_head_ = ((sf_head+1) & (sf_SIZE-1))))
#define sf_incrTail() ((void)(sf_tail_ = ((sf_tail+1) & (sf_SIZE-1))))
#define sf_Pred(a) ((((u8)(a))-1) & (sf_SIZE-1))
#define sf_Succ(a) ((((u8)(a))+1) & (sf_SIZE-1))
#define sf_isEmpty() (sf_head_ == sf_tail)
#define sf_isFull()  (sf_head_ == (((sf_tail+1) & (sf_SIZE-1))))

typedef struct sf T;


static bool comp_fun(const T *a, const T *b) {
  return a->when_to_transmit_ts >= b->when_to_transmit_ts;
}

static bool sf_append(const T *msg) {
  if (sf_isFull())
    return false;
  u8 i = sf_tail;
  u8 k = sf_Pred(i);
  sf_incrTail();

  while (i != sf_head) {
    if (comp_fun(msg, &sf[k])) {
      break;
    }
    sf[i] = sf[k];
    i = k;
    k = sf_Pred(i);
  }

  sf[i] = *msg;

  return true;
}


int fer_tx_get_msgPendingCount() { return (sf_tail_ + sf_SIZE - sf_head_) & (sf_SIZE - 1); }

struct sf *fer_tx_nextMsg() {
  return (sf_isEmpty() ? 0 : &sf[sf_head]);
}
void fer_tx_popMsg() {
  if (sf_isEmpty())
    return;
  sf_incrHead();
}
bool fer_tx_pushMsg(const struct sf *msg) {
  return sf_append(msg);
}
