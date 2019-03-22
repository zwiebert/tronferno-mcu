/*
 * wifi.h
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#ifndef SRC_MAIN_WIFI_H_
#define SRC_MAIN_WIFI_H_

// interface implemented by mcu specific code
void wifi_ip_addr_as_string(char *buf, unsigned buf_len);

// interface called by mcu specific code
void wifi_connected(void);
void wifi_disconnected(void);


#endif /* SRC_MAIN_WIFI_H_ */
