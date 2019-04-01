/*
 * wifi.c
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#include "user_config.h"
#include "ipnet.h"
#include "userio/status_output.h"
#include "http_server.h"
#include "config/config.h"


void ipnet_connected(void) {
  so_output_message(SO_INET_PRINT_ADDRESS, 0);
  hts_enable_http_server(C.http_enable);
}

void ipnet_disconnected(void) {
  hts_enable_http_server(false);
}
