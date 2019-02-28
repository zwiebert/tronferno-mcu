// -*- C -*-
#ifndef FER_H
#define FER_H

#include "fer_code.h"
#include <time.h>
#include "fer_frame.h"
#include "fer_print.h"
#include "fer_prg.h"
#include "fer_sender.h"
#include "fer_transceiver.h"

typedef enum {
	fer_OK, fer_PAIR_NOT_EQUAL, fer_BAD_WORD_PARITY, fer_BAD_CHECKSUM
} fer_error;


//cast data (message after cmd) to byte array
#define fmsg_get_data(msg) ((uint8_t(*)[bytesPerPrgLine])(msg)->rtc)
typedef uint8_t(*fmsg_data)[bytesPerPrgLine];

extern struct fer_msg message_buffer;
#define rxmsg (&message_buffer)
#define txmsg (&message_buffer)
#define rxbuf ((uint8_t *)rxmsg)
#define txbuf ((uint8_t *)txmsg)
#define rxdat fmsg_get_data(rxmsg)
#define txdat fmsg_get_data(txmsg)

// defined in MCU specific code
bool mcu_get_rxPin();
void frx_tick(void);
void mcu_put_txPin(bool dat);

// fernotron.c
void loop(void);
int main_setup(void);

#endif // FER_H
