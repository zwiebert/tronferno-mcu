/*
 * cli_fer.h
 *
 *  Created on: 19.02.2020
 *      Author: bertw
 */

#pragma once

#include "fernotron/fsb.h"

bool cli_parm_to_ferCMD(const char *token, fer_cmd *cmd);
bool asc2group(const char *s, fer_grp *grp);
bool asc2memb(const char *s, fer_memb *memb);


