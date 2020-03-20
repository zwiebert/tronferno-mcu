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

// lock buffer for transmitting
bool ftrx_lockBuffer(bool enableLock);  // blocks receiver access to shared buffer when transmitting



//cast data (message after cmd) to byte array
#define fmsg_get_data(msg) ((uint8_t(*)[FER_PRG_BYTE_CT])(msg)->rtc)
typedef uint8_t (*fmsg_data)[FER_PRG_BYTE_CT];

// message buffer (shared for tx/rx to save RAM)
extern struct fer_raw_msg message_buffer;
extern volatile uint8_t msgBuf_requestLock;
extern volatile bool msgBuf_isLocked;

#define rxmsg (&message_buffer)
#define rxbuf ((uint8_t *)rxmsg)
#define rxdat fmsg_get_data(rxmsg)

#define txmsg (&message_buffer)
#define txbuf ((uint8_t *)txmsg)
#define txdat fmsg_get_data(txmsg)

#endif // FER_H
