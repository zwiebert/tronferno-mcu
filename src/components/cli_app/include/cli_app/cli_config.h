/*
 * cli_config.h
 *
 *  Created on: 20.07.2019
 *      Author: bertw
 */

#pragma once

#include "userio_app/status_output.h"

#define ARG_NW_NONE "none"
#define ARG_NW_WLAN "wlan"
#define ARG_NW_WLAN_AP "ap"
#define ARG_NW_LAN "lan"

extern const char * const cfg_keys[SO_CFG_size];
extern const char * const cfg_args_network[];
extern const char * const cfg_args_lanPhy[];
