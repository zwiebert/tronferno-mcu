// -*- C -*-
#ifndef FER_H
#define FER_H

#include "fernotron/fer_code.h"
#include <time.h>
#include "fer_frame.h"
#include "fer_print.h"
#include "fer_prg.h"
#include "fer_sender.h"
#include "fer_transceiver.h"
#include "fer_imp.h" // FIXME

typedef enum {
	fer_OK, fer_PAIR_NOT_EQUAL, fer_BAD_WORD_PARITY, fer_BAD_CHECKSUM
} fer_error;


//cast data (message after cmd) to byte array
#define fmsg_get_data(msg) ((uint8_t(*)[bytesPerPrgLine])(msg)->rtc)
typedef uint8_t(*fmsg_data)[bytesPerPrgLine];

// message buffer (shared for tx/rx to save RAM)
extern struct fer_msg message_buffer;

#define rxmsg (&message_buffer)
#define rxbuf ((uint8_t *)rxmsg)
#define rxdat fmsg_get_data(rxmsg)

#define txmsg (&message_buffer)
#define txbuf ((uint8_t *)txmsg)
#define txdat fmsg_get_data(txmsg)



#endif // FER_H
