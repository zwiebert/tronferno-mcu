/*
 * cli_fer.h
 *
 *  Created on: 19.02.2020
 *      Author: bertw
 */

#ifndef CLI_CLI_FER_H_
#define CLI_CLI_FER_H_

#include "cli_imp.h"
#include "fernotron/fsb.h"

bool cli_parm_to_ferCMD(const char *token, fer_cmd *cmd);
fsbT *get_sender_by_addr(long addr);

bool asc2group(const char *s, fer_grp *grp);
bool asc2memb(const char *s, fer_memb *memb);


extern gm_bitmask_t manual_bits;

#endif /* CLI_CLI_FER_H_ */
