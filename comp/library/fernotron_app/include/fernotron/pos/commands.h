/*
 * commands.h
 *
 *  Created on: 09.02.2020
 *      Author: bertw
 */

#pragma once

#include "fernotron/trx/raw/fer_msg_plain.h"
#include <stdint.h>
#include "stdbool.h"
#include "shutter_pct.h"


bool fer_cmd_sendShutterCommand(uint32_t a, uint8_t g, uint8_t m, fer_if_cmd cmd, uint8_t repeats);

bool fer_cmd_moveShutterToPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct, uint8_t repeats);
bool fer_if_cmd_moveShuttersToPct(uint32_t a, Fer_GmBitMask *mm, uint8_t pct, uint8_t repeats);

