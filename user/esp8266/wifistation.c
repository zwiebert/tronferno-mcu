#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <espconn.h>
#include <mem.h>

#include "driver/uart.h"
#include "main/config.h"
#include "main/inout.h"
#include "time.h"
#include "main/rtc.h"

#define TCP_HARD_TIMEOUT  (60 * 10)  // terminate connections to avoid dead connections piling up
#define SERIAL_ECHO 0
#define EXP1 1

#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif



// WIFI Station ////////////////////////////////////////

void ICACHE_FLASH_ATTR
user_set_station_config(void) {
  struct station_config stationConf;
  stationConf.bssid_set = 0; //need not check MAC address of AP
#ifdef USE_WLAN
  os_memcpy(&stationConf.ssid, C.wifi_SSID, 32);
  os_memcpy(&stationConf.password, C.wifi_password, 64);
#endif
  wifi_station_set_config(&stationConf);
}

void wst_reconnect(void) {
  uint8_t status = wifi_station_get_connect_status();
  io_printf_fun("wifi state: %d\n", (int) 0xff & status);

  //wifi_station_connect();
}



void ICACHE_FLASH_ATTR
setup_wifistation(void) {
  wifi_set_opmode(STATION_MODE);
  user_set_station_config();
}
