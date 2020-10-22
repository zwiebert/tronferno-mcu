/**
 * \file         fernotron/cuas/cuid_auto_set.h
 * \brief        Set our default central unit id from a received RF message.
 *
 *               The user can set the central unit address/id by just pressing STOP on his original central unit.
 */
#pragma once

#include "stdbool.h"
#include "app_config/proj_app_cfg.h"

/**
 * \brief          Event callback: CUAS-mode had been enabled or disabled
 * \param enabled  true: was enabled, false: was disabled
 * \param cu       New cu-address or 0.
 */
extern void (*fer_cuas_enable_disable_cb)(bool enable, uint32_t cu);

/**
 * \brief      State of the module
 */
typedef enum  {
  FER_CUAS_NONE, ///< Inactive
  FER_CUAS_SCANNING, ///< Scanning in progress
  FER_CUAS_TIME_OUT, ///< Scanning has timeout out
  FER_CUAS_SUCCESS ///< RF message was received and new CU address set from its address
} fer_cuas_state_T;

/**
 * \brief              Start scanning RF messages for central unit
 * \param td           Target descriptor for output
 * \param msg_id       Message ID of CLI command initiating this scan.
 * \param timout_secs  Operation will be stopped after the timout is reached
 * \return             true if scanning is now active
 */
bool fer_cuas_set(const struct TargetDesc &td, uint16_t msg_id, unsigned timeout_secs);


/**
 * \brief             Terminate auto-set mode if timeout has been reached
 */
void fer_cuas_set_check_timeout(void);

/**
 * \brief             Call this if a RF message was received while in auto-set mode
 * \param cu           3-byte-address extracted from the received RF message
 * \return             true on success
 */
bool fer_cuas_set_check(const u32 cu);

/**
 * \brief     Get current state of this module by polling
 * \return    current state. Any value of /link fer_cuas_state_T /endlink
 */
fer_cuas_state_T fer_cuas_getState();



