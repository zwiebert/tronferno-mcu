/*
 * fer_rawmsg_buffer.c
 *
 *  Created on: 19.03.2020
 *      Author: bertw
 */

#include "fernotron/fer_msg_attachment.h"
#include "fer_app_cfg.h"
#include "fer_rawmsg_buffer.h"
#include "int_timer.h"
#include "stdbool.h"
#include "app/common.h"

struct fer_raw_msg rawmsg_buf_rx, rawmsg_buf_tx;
