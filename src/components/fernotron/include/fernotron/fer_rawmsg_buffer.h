// -*- C -*-
#ifndef FER_H
#define FER_H

#include <stdint.h>
#include "stdbool.h"
#include "fernotron/fer_msg_attachment.h"

#include "fernotron/fer_rawmsg_build.h"
#include "fernotron/fer_msg_attachment.h"
#include "fer_msg_rx.h"
#include "fernotron/fer_radio_trx.h"

typedef enum {
  fer_OK, fer_PAIR_NOT_EQUAL, fer_BAD_WORD_PARITY, fer_BAD_CHECKSUM
} fer_error;

//cast data (message after cmd) to byte array
#define fmsg_get_data(msg) ((uint8_t(*)[FER_PRG_BYTE_CT])(msg)->rtc)
typedef uint8_t (*fmsg_data)[FER_PRG_BYTE_CT];

extern struct fer_raw_msg rawmsg_buf_rx, rawmsg_buf_tx;
extern volatile uint8_t msgBuf_requestLock;
extern volatile bool msgBuf_isLocked;

#define rxmsg (&rawmsg_buf_rx)
#define rxbuf ((uint8_t *)rxmsg)
#define rxdat fmsg_get_data(rxmsg)

#define txmsg (&rawmsg_buf_tx)
#define txbuf ((uint8_t *)txmsg)
#define txdat fmsg_get_data(txmsg)

#endif // FER_H
