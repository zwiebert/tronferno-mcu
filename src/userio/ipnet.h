/*
 * wifi.h
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#ifndef SRC_MAIN_IPNET_H_
#define SRC_MAIN_IPNET_H_

#include "lwip/ip4_addr.h"

extern ip4_addr_t ip4_address, ip4_gateway_address, ip4_netmask;

// interface implemented by mcu specific code
void ipnet_addr_as_string(char *buf, unsigned buf_len);

// interface called by mcu specific code
void ipnet_connected(void);
void ipnet_disconnected(void);
bool ipnet_isConnected(void);


#endif /* SRC_MAIN_IPNET_H_ */
