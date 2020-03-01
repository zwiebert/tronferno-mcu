// -*- C -*-
#ifndef FER_H
#define FER_H

#include <time.h>
#include "include/fernotron/fer_msg_basic.h"
#include "include/fernotron/fer_msg_extension.h"

#include "fer_prg.h"
#include "fernotron/fer_msg_extension.h"
#include "fer_send_msg.h"
#include "pins.h" // FIXME
#include "transceiver.h"

typedef enum {
	fer_OK, fer_PAIR_NOT_EQUAL, fer_BAD_WORD_PARITY, fer_BAD_CHECKSUM
} fer_error;


//cast data (message after cmd) to byte array
#define fmsg_get_data(msg) ((uint8_t(*)[FER_PRG_BYTE_CT])(msg)->rtc)
typedef uint8_t(*fmsg_data)[FER_PRG_BYTE_CT];

// message buffer (shared for tx/rx to save RAM)
extern struct fer_msg message_buffer;

#define rxmsg (&message_buffer)
#define rxbuf ((uint8_t *)rxmsg)
#define rxdat fmsg_get_data(rxmsg)

#define txmsg (&message_buffer)
#define txbuf ((uint8_t *)txmsg)
#define txdat fmsg_get_data(txmsg)



#endif // FER_H
