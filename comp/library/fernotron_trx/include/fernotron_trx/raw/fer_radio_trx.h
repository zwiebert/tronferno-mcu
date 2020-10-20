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

#ifdef __cplusplus
  extern "C" {
#endif

  /**
   * \brief  Event callback:  A message has been received
   * \note   The callback must be placed in IRAM, because its called from ISR
   */
extern void (*fer_rx_MSG_RECEIVED_ISR_cb)(void);
/**
 * \brief  Event callback:  A message has been completely transmitted
 * \note   The callback must be placed in IRAM, because its called from ISR
 */
extern void (*fer_tx_MSG_TRANSMITTED_ISR_cb)(void);


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

#ifdef DISTRIBUTION
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
 * \brief     Do some work in transmitting a message
 * \note      Should be called from a timer ISR with  200us (= 5kHz)
 * \note      Timing is less critical, because the output pin is set separately by \link fer_tx_setOutput \endlink.
 */
void fer_tx_dck(void);
/**
 * \brief           Set RF output pin according to the state in the transmitter
 * \note            Calls this from the top of a precise timer ISR.
 */
bool fer_tx_setOutput(void);


/**
 * \brief          Transmit an RF message asynchronously
 * \param msg      Buffer holding the message. Must be valid until \link fer_tx_MSG_TRANSMITTED_ISR_cb \endlink is called.
 * \param msg_type Message type (=size) to send
 */
void fer_tx_transmitFerMsg(fer_rawMsg *msg, fer_msg_type msg_type);

/**
 * \brief            Sample input pin and store the value in the receiver
 * \param pin_level  Current level of input pin
 * \note             Call this from top of a precise timer ISR handler
 */

void fer_rx_sampleInput(bool pin_level);
/**
 * \brief            Do some work in receiving a message
 * \note             Call this from timer ISR with 200us / FREQ_MULT (= 5kHz * FREQ_MULT).
 * \note             Because the sampling was done by \link fer_rx_sampleInput \endlink earlier, the timing is not so critical.
 *                   So it can be called at the end of the ISR or maybe even after the ISR (yield from ISR) XXX
 */
void fer_rx_tick(void);

#ifdef __cplusplus
  }
#endif

