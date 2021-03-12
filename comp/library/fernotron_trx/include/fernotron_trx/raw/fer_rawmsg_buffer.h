/**
 * \file   fernotron_trx/raw/fer_rawmsg_buffer.h
 * \brief  Global static buffers for raw messages.
 */

#pragma once

#include "fernotron_trx/raw/fer_msg_attachment.h"

extern struct fer_raw_msg fer_rawMsg_buf_tx; ///< Buffer for building and sending messages
#define fer_tx_msg (&fer_rawMsg_buf_tx) ///< Alias / address

extern struct fer_raw_msg fer_rawMsg_buf_rx; ///< Buffer for receiving messages
#define fer_rx_msg (&fer_rawMsg_buf_rx) ///< Alias / address


