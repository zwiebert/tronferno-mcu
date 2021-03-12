/**
 * \file  fernotron_trx/raw/fer_msg_type.h
 * \brief kinds of messages
 */
#pragma once

#include <stdint.h>

/// \brief  Message kinds
typedef enum {
  MSG_TYPE_NONE,  ///< no message
  MSG_TYPE_PLAIN, ///< plain message
  MSG_TYPE_PLAIN_DOUBLE, ///< plain message received multiple times (received message with unchanged \link Fer_Cmd::tgl \endlink value)
  MSG_TYPE_RTC, ///< rtc-only message
  MSG_TYPE_TIMER ///< full length timer programming message
} fer_msg_type;

