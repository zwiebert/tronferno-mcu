#pragma once

#include <misc/int_types.h>
#include "fernotron/trx/raw/fer_fsb.h"
#include "fernotron/trx/raw/fer_msg_attachment.h"

struct Fer_Trx_IncomingEvent {
  fer_rawMsg *raw = 0;
  fer_sbT fsb = {};
  fer_msg_type kind:3 = MSG_TYPE_NONE;
  bool tx:1 = false;
  bool first:1 = false;
};
