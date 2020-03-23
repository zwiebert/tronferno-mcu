/*
 * wifi.h
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#ifndef SRC_MAIN_IPNET_H_
#define SRC_MAIN_IPNET_H_

// interface implemented by mcu specific code
void ipnet_addr_as_string(char *buf, unsigned buf_len);

// interface called by mcu specific code
void ipnet_connected(void);
void ipnet_disconnected(void);
bool ipnet_isConnected(void);

uint32_t get_ip4addr_local();
uint32_t get_ip4addr_gateway();
uint32_t get_ip4netmask();

#endif /* SRC_MAIN_IPNET_H_ */
