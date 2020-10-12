/**
 * \brief      Mode for setting shutter end positions
 * \date       28.11.2017
 * \author     bertw
 *
 */

#pragma once

#include "stdbool.h"
#include "app/config/proj_app_cfg.h"
#include <fernotron/trx/fer_trx_c_api.h>




extern void (*fer_sep_enable_disable_cb)(bool enable);

/**
 * \brief          Enable set-end-position-mode
 * \param a        Address of motor to set position to (XXX: we should only use motor addresses here, because all others addresses could address more than one motor)
 * \param g,m      FIXME: this should better not be used
 * \param cmd      FIXME: xUP=set upper end position, xDOWN=set lower end position
 * \return         true for success
 */
bool fer_sep_enable(const struct TargetDesc &td, u32 a, u8 g, u8 m, fer_if_cmd cmd);


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




