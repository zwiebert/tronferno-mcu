/*
 * commands.h
 *
 *  Created on: 09.02.2020
 *      Author: bertw
 */

#pragma once

#include "fernotron/fer_msg_plain.h"
#include <stdint.h>
#include "stdbool.h"
#include "shutter_pct.h"


bool commands_sendShutterCommand(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd, uint8_t repeats);

bool commands_moveShutterToPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct, uint8_t repeats);
bool commands_moveShuttersToPct(uint32_t a, GmBitMask *mm, uint8_t pct, uint8_t repeats);

