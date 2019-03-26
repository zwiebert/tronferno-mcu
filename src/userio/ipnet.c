/*
 * wifi.c
 *
 *  Created on: 20.03.2019
 *      Author: bertw
 */

#include "user_config.h"
#include "ipnet.h"
#include "userio/status_output.h"


void ipnet_connected(void) {
  so_output_message(SO_INET_PRINT_ADDRESS, 0);
}

void ipnet_disconnected(void) {

}
