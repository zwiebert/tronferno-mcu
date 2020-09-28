#pragma once

#include <stdint.h>
#include "stdbool.h"
#include "fernotron/trx/raw/fer_msg_attachment.h"

#include "fernotron/trx/raw/fer_rawmsg_build.h"
#include "fernotron/trx/raw/fer_msg_attachment.h"
#include "fer_msg_rx.h"
#include "fernotron/trx/raw/fer_radio_trx.h"

typedef enum {
  fer_OK, fer_PAIR_NOT_EQUAL, fer_BAD_WORD_PARITY, fer_BAD_CHECKSUM
} fer_error;

extern struct fer_raw_msg fer_rawMsg_buf_rx, fer_rawMsg_buf_tx;
#define fer_rx_msg (&fer_rawMsg_buf_rx)
#define fer_tx_msg (&fer_rawMsg_buf_tx)

