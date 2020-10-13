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

#include "fernotron/trx/raw/fer_msg_plain.h"
#include <stdint.h>
#include "stdbool.h"
#include "shutter_pct.h"


bool fer_cmd_sendShutterCommand(uint32_t a, uint8_t g, uint8_t m, fer_if_cmd cmd, uint8_t repeats);

bool fer_cmd_moveShutterToPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct, uint8_t repeats);
bool fer_if_cmd_moveShuttersToPct(uint32_t a, Fer_GmSet *mm, uint8_t pct, uint8_t repeats);

