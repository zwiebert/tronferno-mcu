/*
 * not_implemented.c
 *
 *  Created on: 12.09.2017
 *      Author: bertw
 */
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "string.h"
#include <ctype.h>

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "time.h"

#include "../../userio/inout.h"
#include "main/rtc.h"

#include "config/config.h"
#include "main/rtc.h"
#include "config/config.h"


//#include <sntp.h>

#define printf ets_printf

static void set_server_by_config() { // FIXME: take list of servers instead only one
  int server_number = 0;


  for (server_number = 0; server_number < 1; ++server_number) {
    const char *server = C.ntp_server;
    bool use_dhcp = strcmp(server, "dhcp") == 0;
    bool use_ipaddr = isdigit((int)server[0]);

    sntp_servermode_dhcp(use_dhcp);

    if (use_ipaddr) {
      ip_addr_t addr = {0};
      ipaddr_aton(server, &addr);
      sntp_setserver(server_number, &addr);
    } else if (use_dhcp) {
      continue;
    } else {
      sntp_setservername(server_number, server);
    }
  }
}

void setup_ntp(void) {
  static int once;
  if (once == 0) {
    once = 1;
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    set_server_by_config();
    sntp_init();

  }
}
