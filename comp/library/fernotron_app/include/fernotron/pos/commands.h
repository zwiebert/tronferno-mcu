/**
 * \file     fernotron/pos/commands.h
 * \brief    commands to move or stop a shutter with position support
 *
 *           Each command will result in:
 *              - the related RF commands to be sent
 *              - keeping track of any movement for user feedback
 *              - store the reached end position
 */

#pragma once

#include "fernotron_trx/raw/fer_msg_plain.h"
#include <stdint.h>
#include "stdbool.h"
#include "shutter_pct.h"

/**
 * \brief         Push command to RF transmitter queue and keep track of any moving and stopping
 * \param a       Device address. If not a central unit, then try to map it using the alias table.
 * \param g,m     Group and member, if device address is central unit. Otherwise pass 0.
 * \param cmd     Command to send
 * \param repeats Repeat mess N times
 * \return        success
 */
bool fer_cmd_sendShutterCommand(uint32_t a, uint8_t g, uint8_t m, fer_if_cmd cmd, uint8_t repeats);

/**
 * \brief         Generate commands to move shutter to given position. Keeps track of all moving and stopping.
 * \param a       Device address. If not out central unit, then look it up in alias table.
 * \param g,m     Group and member, if device address is central unit. Otherwise pass 0.
 * \param pct     Position (int %) to move to.
 * \param repeats Repeat mess N times
 * \return        success
 */
bool fer_cmd_moveShutterToPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct, uint8_t repeats);

/**
 * \brief         Generate commands to move multiple shutters to a given position. Keeps track of all moving and stopping.
 * \param a       Device address.
 * \param mm      Set of groups and members to move.
 * \param pct     Position (int %) to move to.
 * \param repeats Repeat mess N times
 * \return        success
 */
bool fer_if_cmd_moveShuttersToPct(uint32_t a, Fer_GmSet *mm, uint8_t pct, uint8_t repeats);

