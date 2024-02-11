/*
 * fer_rawmsg_buffer.c
 *
 *  Created on: 19.03.2020
 *      Author: bertw
 */

#include "fernotron_trx/raw/fer_msg_attachment.h"
#include "fer_app_cfg.h"
#include "fernotron_trx/raw/fer_rawmsg_buffer.h"
#include "fernotron_trx/isr_timer_config.h"


struct fer_raw_msg fer_rawMsg_buf_rx, fer_rawMsg_buf_tx;
