/*
 * commands.h
 *
 *  Created on: 09.02.2020
 *      Author: bertw
 */

#ifndef SRC_SHUTTER_POSITIONS_COMMANDS_H_
#define SRC_SHUTTER_POSITIONS_COMMANDS_H_

#include "fernotron/fer_msg_basic.h"
#include <stdint.h>
#include "stdbool.h"

bool commands_moveShutterToPct(uint32_t a, uint8_t g, uint8_t m, uint8_t pct, uint8_t repeats);
bool commands_sendShutterCommand(uint32_t a, uint8_t g, uint8_t m, fer_cmd cmd, uint8_t repeats);

#endif /* SRC_SHUTTER_POSITIONS_COMMANDS_H_ */