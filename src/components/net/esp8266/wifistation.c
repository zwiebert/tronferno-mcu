#include "user_interface.h"
#include <os_type.h>
#include <ets_sys.h>
#include <ets_sys.h>
#include <osapi.h>
#include <gpio.h>
#include <espconn.h>
#include <mem.h>

#include "driver/uart.h"
#include "config/config.h"
#include "time.h"

#include "txtio/inout.h"
#include "net/ipnet.h"
#include "app/rtc.h"


#define printf io_printf_fun
#ifndef DISTRIBUTION
#define D(x) x
#else
#define D(x)
#endif

extern struct ip_addr ip4_address, ip4_gateway_address, ip4_netmask;


// WIFI Station ////////////////////////////////////////

void 
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
  u8 status = wifi_station_get_connect_status();
  io_printf_fun("wifi state: %d\n", (int) 0xff & status);

  //wifi_station_connect();
}

void wifi_handle_event_cb(System_Event_t *evt) {
  D(printf("event %x\n", evt->event));
  switch (evt->event) {
  case EVENT_STAMODE_CONNECTED:
    D(printf("connect to ssid %s, channel %d\n", evt->event_info.connected.ssid, evt->event_info.connected.channel));
    break;
  case EVENT_STAMODE_DISCONNECTED:
    D(printf("disconnect from ssid %s, reason %d\n", evt->event_info.disconnected.ssid, evt->event_info.disconnected.reason));
    ipnet_disconnected();
    break;
  case EVENT_STAMODE_AUTHMODE_CHANGE:
    D(printf("mode: %d -> %d\n", evt->event_info.auth_change.old_mode, evt->event_info.auth_change.new_mode));
    break;
  case EVENT_STAMODE_GOT_IP:
    D(printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR, IP2STR(&evt->event_info.got_ip.ip), IP2STR(&evt->event_info.got_ip.mask), IP2STR(&evt->event_info.got_ip.gw)); printf("\n"));
    ip4_address = evt->event_info.got_ip.ip;
    ip4_gateway_address = evt->event_info.got_ip.gw;
    ip4_netmask = evt->event_info.got_ip.mask;

    ipnet_connected();
    break;
  case EVENT_SOFTAPMODE_STACONNECTED:
    D(printf("station: " MACSTR "join, AID = %d\n", MAC2STR(evt->event_info.sta_connected.mac), evt->event_info.sta_connected.aid)); break;
  case EVENT_SOFTAPMODE_STADISCONNECTED:
    D(printf("station: " MACSTR "leave, AID = %d\n", MAC2STR(evt->event_info.sta_disconnected.mac), evt->event_info.sta_disconnected.aid)); break;
  default:
    break;
  }
}

void 
wifistation_setup(void) {
  wifi_set_opmode(STATION_MODE);
  user_set_station_config();
  wifi_set_event_handler_cb(wifi_handle_event_cb);
}
