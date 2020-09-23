/*
 * fer_print.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#pragma once

#include "fernotron/fer_msg_attachment.h"

void fer_msg_print(const char *tag, const fer_rawMsg *msg, fer_msg_type t, bool verbose);
void fer_msg_print_as_cmdline(const char *tag, const fer_rawMsg *msg, fer_msg_type t);


