/**
 * \file   fernotron_trx/raw/fer_radio_trx.h
 * \brief  RF transceiver
 * \author bertw
 */

#pragma once

#include <stdint.h>
#include "stdbool.h"
#include "app_config/proj_app_cfg.h"
#include "fer_msg_attachment.h"
#include <fernotron_trx/fer_trx_c_api.h>

#ifdef __cplusplus
  extern "C" {
#endif

  /// \brief Kinds of supported RF hardware.
  enum rfTrx {
    rfTrxNone, rfTrxRxTx, rfTrxRxItx, rfTrxCc1101,
  };

/**
 * \brief  Kind of received Message
 */
extern volatile fer_msg_type fer_rx_messageReceived;
/**
 * \brief If  RF transmitter is free (it's not busy with completing the previous message)
 */
extern volatile bool fer_tx_messageToSend_isReady;

#define fer_rx_hasNewMsg() (!!fer_rx_messageReceived)
#define fer_tx_isTransmitterBusy() (!!fer_tx_messageToSend_isReady)

#if 0 //def DISTRIBUTION  /// XXX: enable receiving messages sent by ourself (to diagnose bad transmitter modules. IMPROVE THIS CODE)
#define fer_rx_isReceiverBlocked() (fer_rx_messageReceived || fer_tx_messageToSend_isReady)
#else
#define fer_rx_isReceiverBlocked() (fer_rx_messageReceived)
#endif

//
/**
 * \brief  unlock read buffer after done with data
 * \note  call it after received data buffers has been processed by main thread
 */
void fer_rx_clear(void);

/**
 * \brief  Quality of message receiving for debug purposes
 */
struct fer_rx_quality {
  uint8_t bad_pair_count; ///< Count of non matching byte pairs
};
/**
 * \brief Get quality of latest  message receiving
 */
void fer_rx_getQuality(struct fer_rx_quality *dst);

// tick should be called from stable timer interrupt
// do a bit of work each tick

/**
 * \brief          Transmit an RF message asynchronously
 * \param msg      Buffer holding the message. Must be valid until \link fer_tx_MSG_TRANSMITTED_ISR_cb \endlink is called.
 * \param msg_type Message type (=size) to send
 */
void fer_tx_transmitFerMsg(fer_rawMsg *msg, fer_msg_type msg_type);


#ifdef __cplusplus
  }
#endif

