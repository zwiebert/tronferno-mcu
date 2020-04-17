/*
 * fer_print.h
 *
 *  Created on: 28.02.2019
 *      Author: bertw
 */

#ifndef USER_MAIN_FER_PRINT_H_
#define USER_MAIN_FER_PRINT_H_

#include "fernotron/fer_msg_attachment.h"

void fmsg_print(const char *tag, const fer_rawMsg *msg, fmsg_type t, bool verbose);
void fmsg_print_as_cmdline(const char *tag, const fer_rawMsg *msg, fmsg_type t);


#endif /* USER_MAIN_FER_PRINT_H_ */
