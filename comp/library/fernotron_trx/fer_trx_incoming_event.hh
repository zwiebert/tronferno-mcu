#pragma once

#include <misc/int_types.h>
#include "fernotron/fsb.h"
#include "fernotron/fer_msg_attachment.h"

struct Fer_Trx_IncomingEvent {
  fer_rawMsg *raw = 0;
  fer_sbT fsb = {};
  fer_msg_type kind = MSG_TYPE_NONE;
};
