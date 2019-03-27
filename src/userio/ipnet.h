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


#endif /* SRC_MAIN_IPNET_H_ */
