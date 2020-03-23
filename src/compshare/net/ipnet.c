/*
 * wifi.c
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include "ipnet.h"
//XXX #include "userio_app/status_output.h"
#include "net/http/server/http_server.h"
#include "config/config.h"
#include "misc/int_types.h"

void main_setup_ip_dependent(void);
u32 ip4_address, ip4_gateway_address, ip4_netmask;

u32 get_ip4addr_local();
u32 get_ip4addr_gateway();
u32 get_ip4netmask();


static bool Is_connected;

void ipnet_connected(void) {
  Is_connected = true;
 //XXX so_output_message(SO_INET_PRINT_ADDRESS, 0);
  main_setup_ip_dependent();
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
