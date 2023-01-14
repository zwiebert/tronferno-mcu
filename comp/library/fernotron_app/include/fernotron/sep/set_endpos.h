/**
 * \file       fernotron/sep/set_endpos.h
 * \brief      Mode for setting shutter end positions
 * \date       28.11.2017
 * \author     bertw
 *
 */

#pragma once

#include "stdbool.h"

#include <fernotron_trx/fer_trx_c_api.h>

/**
 * \brief          Event callback: SEP-mode had been enabled or disabled
 * \param enabled  true: was enabled, false: was disabled
 */
extern void (*fer_sep_enable_disable_cb)(bool enable);

const int fer_sep_TIMEOUT = 180; // timeout to switch off SEP mode
const int fer_sep_BUTTON_TIMEOUT = 3; // timeout to send stop if no movement function was called repeatedly.
const int SEP_AUTH_TIMEOUT_SECS = 320;
const int SEP_AUTH_BUTTON_TIMEOUT_SECS = 60;

bool fer_sep_authenticate(const class UoutWriter &td, uint32_t auth_key, int auth_timeout_secs = SEP_AUTH_TIMEOUT_SECS, int button_timeout_secs =
    SEP_AUTH_BUTTON_TIMEOUT_SECS);
bool fer_sep_deauthenticate(const class UoutWriter &td, uint32_t auth_key);

/**
 * \brief          Enable set-end-position-mode
 * \param a        Radio code or central-unit-ID. WARNING: Only the radio code is always unique, while more than one motor can be registered under the same receiver number.
 * \param g,m      Group/receiver number if A is a central-unit-ID.
 * \param enable_timeout_secs  timeout to switch off SEP mode
 * \param button_timeout_secs  timeout to send stop if no movement function was called repeatedly.
 * \return         true for success
 */
bool fer_sep_enable(const class UoutWriter &td, uint32_t auth_key, uint32_t a, uint8_t g = 0, uint8_t m = 0, int enable_timeout_secs = fer_sep_TIMEOUT, int button_timeout_secs =
    fer_sep_BUTTON_TIMEOUT);

bool fer_sep_move_up(uint32_t auth_key = 0);
bool fer_sep_move_down(uint32_t auth_key = 0);
bool fer_sep_move_continue(uint32_t auth_key = 0);
bool fer_sep_move_stop(uint32_t auth_key = 0);
bool fer_sep_move_test();

/**
 * \brief          Disable set-end-position-mode
 */
void fer_sep_disable(void);

/**
 * \brief         Do work
 */
void fer_sep_loop(void);

/**
 * \brief         Test if set-end-position-mode is active
 */
bool fer_sep_is_enabled(void);

