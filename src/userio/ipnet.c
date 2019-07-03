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




ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

void
ipnet_addr_as_string(char *buf, unsigned buf_len) {
  ip4addr_ntoa_r(&ip4_address, buf, buf_len);
}

static bool Is_connected;

void ipnet_connected(void) {
  Is_connected = true;
  so_output_message(SO_INET_PRINT_ADDRESS, 0);
#ifdef USE_HTTP
  hts_enable_http_server(C.http_enable);
#endif
}

void ipnet_disconnected(void) {
  Is_connected = false;
#ifdef USE_HTTP
  hts_enable_http_server(false);
#endif
}

bool ipnet_isConnected(void) {
  return Is_connected;
}
