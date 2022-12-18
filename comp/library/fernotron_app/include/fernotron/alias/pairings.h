/**
 * \file fernotron/alias/pairings.h
 * \brief Map Fernotron controller 3-byte-addresses to an a/g/m addresses.
 *
 * Any controller paired with a receiver should also be paired in this API, so we can apply any commands sent by it to the receiver's state stored in the MCU.
 */
#pragma once

#include "fernotron/fer_pct.h"
#include "app_config/proj_app_cfg.h"

#include <stdlib.h>
#include "stdbool.h"

enum fer_alias_cmds { PC_none, PC_pair, PC_unpair, PC_read };

/**
 * \brief               Pair or un-pair a CONTROLLER to g/m.
 * \param controller    3-byte-address of a controller (e.g. sun-sensor)
 * \param g,m           Group/Member. 0 means "any".
 * \param unpair        If true, remove the pairing if it exists
 * \return              true on success
 */
bool fer_alias_controller(uint32_t controller, uint8_t g, uint8_t m, bool unpair);


/**
 * \brief               Pair or un-pair a CONTROLLER by bit-mask
 * \param controller    3-byte-address of a controller (e.g. sun-sensor)
 * \param gm            Pair for each bit set to 1. Un-pair for each bit set to 0.
 * \return              true on success
 */
bool fer_alias_setControllerPairings(uint32_t controller, Fer_GmSet *gm);

/**
 * \brief               Get current pairings for CONTROLLER
 * \param controller    3-byte-address of a controller (e.g. sun-sensor)
 * \param[out] gm       Set bit for each paired g/m.
 * \return              true on success
 */
bool fer_alias_getControllerPairings(uint32_t controller, /*out*/ Fer_GmSet *gm);

/**
 * \brief               Delete any data we have stored about CONTROLLER. All pairings will be lost.
 * \param controller    3-byte-address of a controller (e.g. sun-sensor)
 * \return              true on success
 */
bool fer_alias_rmController(uint32_t controller);

/**
 * \brief              Print all stored parings to TD
 * \param td           Target descriptor for output
 * \return             true on success
 */
bool fer_alias_so_output_all_pairings(const struct TargetDesc &td);

/**
 * \brief              Start listening on RF receiver for controller to pair/unpair
 * \param g,m          Group/Member to pair to. 0 means "any".
 * \param c            If received a RF command, then do this command.
 * \return             true on success
 *
 */
bool fer_alias_auto_set(uint8_t g, uint8_t m, fer_alias_cmds c, uint16_t id, unsigned timeout_secs);


// check for result (called from main loop)

/**
 * \brief             Terminate auto-set mode if timeout has been reached
 */
void fer_alias_auto_set_check_timeout(void);
/**
 * \brief             Call this if a RF message was received while in auto-set mode
 * \param a           3-byte-address extracted from the received RF message
 * \return             true on success
 */
bool fer_alias_auto_set_check(uint32_t a);

