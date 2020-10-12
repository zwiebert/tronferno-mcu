/**
 * \file     fernotron/txtio/fer_print.h
 * \brief    Print raw message as formatted text to standard output (for debug purposes).
 */
/*
 * fer_print.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#pragma once

#include "fernotron/trx/raw/fer_msg_attachment.h"
#include <fernotron/trx/fer_trx_api.hh>

void fer_msg_print(const char *tag, const fer_rawMsg *msg, fer_msg_kindT t, bool verbose);
void fer_msg_print_as_cmdline(const char *tag, const fer_rawMsg *msg, fer_msg_kindT t);


