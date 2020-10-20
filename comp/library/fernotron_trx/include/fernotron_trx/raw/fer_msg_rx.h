/*
 * fer_rx_tx.h
 *
 *  Created on: 18.02.2020
 *      Author: bertw
 */

#pragma once

#include "fer_fsb.h"
#include "fer_msg_plain.h"
#include "fernotron_trx/timer_data.h"
#include <time.h>

#ifdef __cplusplus
  extern "C" {
#endif

/**
 * \brief  Do some work
 * \note   Should be called from main thread as reaction to \link fer_rx_MSG_RECEIVED_ISR_cb \endlink.
 */
void fer_rx_loop(void);



#ifdef __cplusplus
  }
#endif
